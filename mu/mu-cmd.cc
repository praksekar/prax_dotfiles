/*
** Copyright (C) 2010-2020 Dirk-Jan C. Binnema <djcb@djcbsoftware.nl>
**
** This program is free software; you can redistribute it and/or modify it
** under the terms of the GNU General Public License as published by the
** Free Software Foundation; either version 3, or (at your option) any
** later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software Foundation,
** Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
**
*/

#include "config.h"

#include <iostream>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "mu-msg.h"
#include "mu-msg-part.h"
#include "mu-cmd.hh"
#include "mu-maildir.h"
#include "mu-contacts.hh"
#include "mu-runtime.h"
#include "mu-flags.h"

#include "utils/mu-util.h"
#include "utils/mu-str.h"
#include "utils/mu-date.h"

#include "utils/mu-error.hh"

#define VIEW_TERMINATOR '\f' /* form-feed */

static gboolean
view_msg_sexp (MuMsg *msg, const MuConfig *opts)
{
	char *sexp;

	sexp = mu_msg_to_sexp (msg, 0, NULL, mu_config_get_msg_options(opts));
	fputs (sexp, stdout);
	g_free (sexp);

	return TRUE;
}


static void
each_part (MuMsg *msg, MuMsgPart *part, gchar **attach)
{
	char *fname, *tmp;

	if (!mu_msg_part_maybe_attachment (part))
		return;

	fname = mu_msg_part_get_filename (part, FALSE);
	if (!fname)
		return;

	tmp = *attach;
	*attach = g_strdup_printf ("%s%s'%s'",
				   *attach ? *attach : "",
				   *attach ? ", " : "",
				   fname);
	g_free (tmp);
}

/* return comma-sep'd list of attachments */
static gchar *
get_attach_str (MuMsg *msg, const MuConfig *opts)
{
	gchar		*attach;

        const auto msgopts = (MuMsgOptions)
                (mu_config_get_msg_options(opts) | MU_MSG_OPTION_CONSOLE_PASSWORD);

	attach = NULL;
	mu_msg_part_foreach (msg, msgopts,
			     (MuMsgPartForeachFunc)each_part, &attach);
	return attach;
}

#define color_maybe(C) do { if(color) fputs ((C),stdout);} while(0)

static void
print_field (const char* field, const char *val, gboolean color)
{
	if (!val)
		return;

	color_maybe (MU_COLOR_MAGENTA);
	mu_util_fputs_encoded (field, stdout);
	color_maybe (MU_COLOR_DEFAULT);
	fputs (": ", stdout);

	if (val) {
		color_maybe (MU_COLOR_GREEN);
		mu_util_fputs_encoded (val, stdout);
	}

	color_maybe (MU_COLOR_DEFAULT);
	fputs ("\n", stdout);
}


/* a summary_len of 0 mean 'don't show summary, show body */
static void
body_or_summary (MuMsg *msg, const MuConfig *opts)
{
	const char *body;
	gboolean    color;
        int my_opts = mu_config_get_msg_options(opts) |
                MU_MSG_OPTION_CONSOLE_PASSWORD;

	color = !opts->nocolor;
	body = mu_msg_get_body_text (msg, (MuMsgOptions)my_opts);
	if (!body) {
		if (mu_msg_get_flags (msg) & MU_FLAG_ENCRYPTED) {
			color_maybe (MU_COLOR_CYAN);
			g_print ("[No body found; "
				 "message has encrypted parts]\n");
		} else {
			color_maybe (MU_COLOR_MAGENTA);
			g_print ("[No body found]\n");
		}
		color_maybe (MU_COLOR_DEFAULT);
		return;
	}

	if (opts->summary_len != 0) {
		gchar *summ;
		summ = mu_str_summarize (body, opts->summary_len);
		print_field ("Summary", summ, color);
		g_free (summ);
	} else {
		mu_util_print_encoded ("%s", body);
		if (!g_str_has_suffix (body, "\n"))
			g_print ("\n");
	}
}


/* we ignore fields for now */
/* summary_len == 0 means "no summary */
static gboolean
view_msg_plain (MuMsg *msg, const MuConfig *opts)
{
	gchar *attachs;
	time_t date;
	const GSList *lst;
	gboolean color;

	color = !opts->nocolor;

	print_field ("From",    mu_msg_get_from (msg),    color);
	print_field ("To",      mu_msg_get_to (msg),      color);
	print_field ("Cc",      mu_msg_get_cc (msg),      color);
	print_field ("Bcc",     mu_msg_get_bcc (msg),     color);
	print_field ("Subject", mu_msg_get_subject (msg), color);

	if ((date = mu_msg_get_date (msg)))
		print_field ("Date", mu_date_str_s ("%c", date),
			     color);

	if ((lst = mu_msg_get_tags (msg))) {
		gchar *tags;
		tags = mu_str_from_list (lst,',');
		print_field ("Tags", tags, color);
		g_free (tags);
	}

	if ((attachs = get_attach_str (msg, opts))) {
		print_field ("Attachments", attachs, color);
		g_free (attachs);
	}

	body_or_summary (msg, opts);

	return TRUE;
}


static gboolean
handle_msg (const char *fname, const MuConfig *opts, GError **err)
{
	MuMsg *msg;
	gboolean rv;

	msg = mu_msg_new_from_file (fname, NULL, err);
	if (!msg)
		return FALSE;

	switch (opts->format) {
	case MU_CONFIG_FORMAT_PLAIN:
		rv = view_msg_plain (msg, opts);
		break;
	case MU_CONFIG_FORMAT_SEXP:
		rv = view_msg_sexp (msg, opts);
		break;
	default:
		g_critical ("bug: should not be reached");
		rv = FALSE;
	}

	mu_msg_unref (msg);

	return rv;
}

static gboolean
view_params_valid (const MuConfig *opts, GError **err)
{
	/* note: params[0] will be 'view' */
	if (!opts->params[0] || !opts->params[1]) {
		mu_util_g_set_error (err, MU_ERROR_IN_PARAMETERS,
				     "error in parameters");
		return FALSE;
	}

	switch (opts->format) {
	case MU_CONFIG_FORMAT_PLAIN:
	case MU_CONFIG_FORMAT_SEXP:
		break;
	default:
		mu_util_g_set_error (err, MU_ERROR_IN_PARAMETERS,
				     "invalid output format");
		return FALSE;
	}

	return TRUE;
}


static MuError
cmd_view (const MuConfig *opts, GError **err)
{
	int i;
	gboolean rv;

	g_return_val_if_fail (opts, MU_ERROR_INTERNAL);
	g_return_val_if_fail (opts->cmd == MU_CONFIG_CMD_VIEW,
			      MU_ERROR_INTERNAL);

	rv = view_params_valid (opts, err);
	if (!rv)
		goto leave;

	for (i = 1; opts->params[i]; ++i) {

		rv = handle_msg (opts->params[i], opts, err);
		if (!rv)
			break;

		/* add a separator between two messages? */
		if (opts->terminator)
			g_print ("%c", VIEW_TERMINATOR);
	}

leave:
	if (!rv)
		return err && *err ? (MuError)(*err)->code : MU_ERROR;

	return MU_OK;
}

static MuError
cmd_mkdir (const MuConfig *opts, GError **err)
{
	int i;

	g_return_val_if_fail (opts, MU_ERROR_INTERNAL);
	g_return_val_if_fail (opts->cmd == MU_CONFIG_CMD_MKDIR,
			      MU_ERROR_INTERNAL);

	if (!opts->params[1]) {
		mu_util_g_set_error (err, MU_ERROR_IN_PARAMETERS,
				     "missing directory parameter");
		return MU_ERROR_IN_PARAMETERS;
	}

	for (i = 1; opts->params[i]; ++i)
		if (!mu_maildir_mkdir (opts->params[i], opts->dirmode,
				       FALSE, err))
			return err && *err ? (MuError)(*err)->code :
				MU_ERROR_FILE_CANNOT_MKDIR;
	return MU_OK;
}


static gboolean
check_file_okay (const char *path, gboolean cmd_add)
{
 	if (!g_path_is_absolute (path)) {
		g_printerr ("path is not absolute: %s\n", path);
		return FALSE;
	}

	if (cmd_add && access(path, R_OK) != 0) {
		g_printerr ("path is not readable: %s: %s\n",
                            path, strerror (errno));
		return FALSE;
	}

	return TRUE;
}

typedef bool (*ForeachMsgFunc) (Mu::Store& store, const char *path, GError **err);

static MuError
foreach_msg_file (Mu::Store& store, const MuConfig *opts,
		  ForeachMsgFunc foreach_func, GError **err)
{
	unsigned	u;
	gboolean	all_ok;

	/* note: params[0] will be 'add' */
	if (!opts->params[0] || !opts->params[1]) {
		g_print ("usage: mu %s <file> [<files>]\n",
			 opts->params[0] ? opts->params[0] : "<cmd>");
		mu_util_g_set_error (err, MU_ERROR_IN_PARAMETERS,
				     "missing parameters");
		return MU_ERROR_IN_PARAMETERS;
	}

	for (u = 1, all_ok = TRUE; opts->params[u]; ++u) {

		const char* path;

		path = opts->params[u];

		if (!check_file_okay (path, TRUE)) {
			all_ok = FALSE;
			g_printerr ("not a valid message file: %s\n", path);
			continue;
		}

		if (!foreach_func (store, path, err)) {
			all_ok = FALSE;
			g_printerr ("error with %s: %s\n", path,
                                    (err&&*err) ? (*err)->message :
                                   "something went wrong");
			g_clear_error (err);
			continue;
		}
	}

	if (!all_ok) {
		mu_util_g_set_error (err, MU_ERROR_XAPIAN_STORE_FAILED,
				     "%s failed for some message(s)",
				     opts->params[0]);
		return MU_ERROR_XAPIAN_STORE_FAILED;
	}

	return MU_OK;
}


static bool
add_path_func (Mu::Store& store, const char *path, GError **err)
{
        const auto docid = store.add_message (path);
        g_debug ("added message @ %s, docid=%u", docid);

        return true;
}


static MuError
cmd_add (Mu::Store& store, const MuConfig *opts, GError **err)
{
	g_return_val_if_fail (opts, MU_ERROR_INTERNAL);
	g_return_val_if_fail (opts->cmd == MU_CONFIG_CMD_ADD,
			      MU_ERROR_INTERNAL);

	return foreach_msg_file (store, opts, add_path_func, err);
}

static bool
remove_path_func (Mu::Store& store, const char *path, GError **err)
{
        const auto res = store.remove_message (path);
        g_debug ("removed %s (%s)", path, res ? "yes" : "no");

        return true;
}

static MuError
cmd_remove (Mu::Store& store, const MuConfig *opts, GError **err)
{
	g_return_val_if_fail (opts, MU_ERROR_INTERNAL);
	g_return_val_if_fail (opts->cmd == MU_CONFIG_CMD_REMOVE,
			      MU_ERROR_INTERNAL);

	return foreach_msg_file (store, opts, remove_path_func, err);
}

static bool
tickle_func (Mu::Store& store, const char *path, GError **err)
{
	MuMsg *msg{mu_msg_new_from_file (path, NULL, err)};
	if (!msg)
		return false;

	const auto res = mu_msg_tickle (msg, err);
        g_debug ("tickled %s (%s)", res ? "ok" : "failed");
	mu_msg_unref (msg);

	return res == TRUE;
}


static MuError
cmd_tickle (Mu::Store& store, const MuConfig *opts, GError **err)
{
	g_return_val_if_fail (opts, MU_ERROR_INTERNAL);
	g_return_val_if_fail (opts->cmd == MU_CONFIG_CMD_TICKLE,
			      MU_ERROR_INTERNAL);

	return foreach_msg_file (store, opts, tickle_func, err);
}

struct _VData {
	MuMsgPartSigStatus combined_status;
	char *report;
	gboolean oneline;
};
typedef struct _VData VData;

static void
each_sig (MuMsg *msg, MuMsgPart *part, VData *vdata)
{
	MuMsgPartSigStatusReport *report;

	report = part->sig_status_report;
	if (!report)
		return;

	if (vdata->oneline)
		vdata->report = g_strdup_printf
			("%s%s%s",
			 vdata->report ? vdata->report : "",
			 vdata->report ? "; " : "",
			 report->report);
	else
		vdata->report = g_strdup_printf
			("%s%s\t%s",
			 vdata->report ? vdata->report : "",
			 vdata->report ? "\n" : "",
			 report->report);

	if (vdata->combined_status == MU_MSG_PART_SIG_STATUS_BAD ||
	    vdata->combined_status == MU_MSG_PART_SIG_STATUS_ERROR)
		return;

	vdata->combined_status = report->verdict;
}


static void
print_verdict (VData *vdata, gboolean color, gboolean verbose)
{
 	g_print ("verdict: ");

	switch (vdata->combined_status) {
	case MU_MSG_PART_SIG_STATUS_UNSIGNED:
		g_print ("no signature found");
		break;
	case MU_MSG_PART_SIG_STATUS_GOOD:
		color_maybe (MU_COLOR_GREEN);
		g_print ("signature(s) verified");
		break;
	case MU_MSG_PART_SIG_STATUS_BAD:
		color_maybe (MU_COLOR_RED);
		g_print ("bad signature");
		break;
	case MU_MSG_PART_SIG_STATUS_ERROR:
		color_maybe (MU_COLOR_RED);
		g_print ("verification failed");
		break;
	case MU_MSG_PART_SIG_STATUS_FAIL:
		color_maybe(MU_COLOR_RED);
		g_print ("error in verification process");
		break;
	default: g_return_if_reached ();
	}

	color_maybe (MU_COLOR_DEFAULT);
	if (vdata->report && verbose)
		g_print ("%s%s\n",
			 (vdata->oneline) ? ";" : "\n",
			 vdata->report);
	else
		g_print ("\n");
}


static MuError
cmd_verify (const MuConfig *opts, GError **err)
{
	MuMsg *msg;
	int    msgopts;
	VData  vdata;

	g_return_val_if_fail (opts, MU_ERROR_INTERNAL);
	g_return_val_if_fail (opts->cmd == MU_CONFIG_CMD_VERIFY,
			      MU_ERROR_INTERNAL);

	if (!opts->params[1]) {
		mu_util_g_set_error (err, MU_ERROR_IN_PARAMETERS,
				     "missing message-file parameter");
		return MU_ERROR_IN_PARAMETERS;
	}

	msg = mu_msg_new_from_file (opts->params[1], NULL, err);
	if (!msg)
		return MU_ERROR;

	msgopts = mu_config_get_msg_options (opts)
		| MU_MSG_OPTION_VERIFY
		| MU_MSG_OPTION_CONSOLE_PASSWORD;

	vdata.report  = NULL;
	vdata.combined_status = MU_MSG_PART_SIG_STATUS_UNSIGNED;
	vdata.oneline = FALSE;

	mu_msg_part_foreach (msg, (MuMsgOptions)msgopts,
			     (MuMsgPartForeachFunc)each_sig, &vdata);

	if (!opts->quiet)
		print_verdict (&vdata, !opts->nocolor, opts->verbose);

	mu_msg_unref (msg);
	g_free (vdata.report);

	return vdata.combined_status == MU_MSG_PART_SIG_STATUS_GOOD ?
		MU_OK : MU_ERROR;
}

static MuError
cmd_info (const Mu::Store& store, const MuConfig *opts, GError **err)
{
	const auto green{opts->nocolor ? "" : MU_COLOR_GREEN};
	const auto def{opts->nocolor ? "" : MU_COLOR_DEFAULT};

        std::cout << "database-path      : "
                  << green << store.database_path() << def << "\n"
                  << "messages in store  : "
                  << green << store.size() << def << "\n"
                  << "schema-version     : "
                  << green << store.schema_version() << def << "\n";

	const auto created{store.created()};
	const auto tstamp{::localtime (&created)};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-y2k"
        char tbuf[64];
	strftime (tbuf, sizeof(tbuf), "%c", tstamp);
#pragma GCC diagnostic pop

        std::cout << "created            : " << green << tbuf << def << "\n"
                  << "maildir            : "
                  << green << store.root_maildir() << def << "\n";

	std::cout << ("personal-addresses : ");

	const auto addrs{store.personal_addresses()};
        if (addrs.empty())
                std::cout << green << "<none>" << def << "\n";
        else {
                bool first{true};
                for (auto&& c: addrs) {
                        std::cout << (!first ?  "                     " : "")
                                  << green << c << def << "\n";
                        first = false;
                }
        }

	return MU_OK;
}

static MuError
cmd_init (const MuConfig *opts, GError **err)
{
        /* not provided, nor could we find a good default */
        if (!opts->maildir) {
                mu_util_g_set_error (err, MU_ERROR_IN_PARAMETERS,
				     "missing --maildir parameter and could "
                                     "not determine default");
		return MU_ERROR_IN_PARAMETERS;
        }

        Mu::StringVec my_addrs;
        auto addrs = opts->my_addresses;
        while (addrs && *addrs)  {
                my_addrs.emplace_back (*addrs);
                ++addrs;
        }

        Mu::Store store(mu_runtime_path(MU_RUNTIME_PATH_XAPIANDB), opts->maildir, my_addrs);

	if (!opts->quiet) {
		cmd_info (store, opts, NULL);
		g_print ("\nstore created.\n"
			 "use 'mu index' to fill the database "
                         "with your messages.\n"
			 "see mu-index(1) for details\n");
	}

	return MU_OK;
}

static MuError
cmd_index (Mu::Store& store, const MuConfig *opts, GError **err)
{
        const auto res = mu_cmd_index(store, opts, err);
        if (res == MU_OK && !opts->quiet)
                cmd_info(store, opts, err);

        return res;
}

static MuError
cmd_find (const MuConfig *opts, GError **err)
{
        Mu::Store store{mu_runtime_path(MU_RUNTIME_PATH_XAPIANDB), true/*readonly*/};

        return mu_cmd_find(reinterpret_cast<MuStore*>(&store), opts, err);
}

static void
show_usage (void)
{
	g_print ("usage: mu command [options] [parameters]\n");
	g_print ("where command is one of index, find, cfind, view, mkdir, "
		   "extract, add, remove, script, verify or server\n");
	g_print ("see the mu, mu-<command> or mu-easy manpages for "
		   "more information\n");
}

typedef MuError (*readonly_store_func) (const Mu::Store&, const MuConfig *, GError **err);
typedef MuError (*writable_store_func) (Mu::Store&,       const MuConfig *, GError **err);


static MuError
with_readonly_store (readonly_store_func func, const MuConfig *opts, GError **err)
{
        const Mu::Store store{mu_runtime_path(MU_RUNTIME_PATH_XAPIANDB), true/*readonly*/};
	return func (store, opts, err);
}

static MuError
with_writable_store (writable_store_func func, const MuConfig *opts, GError **err)
{
        Mu::Store store{mu_runtime_path(MU_RUNTIME_PATH_XAPIANDB), false/*!readonly*/};
	return func (store, opts, err);
}


static gboolean
check_params (const MuConfig *opts, GError **err)
{
	if (!opts->params||!opts->params[0]) {/* no command? */
		show_usage ();
		mu_util_g_set_error (err, MU_ERROR_IN_PARAMETERS,
				     "error in parameters");
		return FALSE;
	}

	return TRUE;
}

MuError
mu_cmd_execute (const MuConfig *opts, GError **err) try
{
	MuError merr;

	g_return_val_if_fail (opts, MU_ERROR_INTERNAL);

	if (!check_params(opts, err))
		return MU_G_ERROR_CODE(err);

	switch (opts->cmd) {

		/* already handled in mu-config.c */
	case MU_CONFIG_CMD_HELP: return MU_OK;

        /* no store needed */

	case MU_CONFIG_CMD_MKDIR:   merr = cmd_mkdir      (opts, err); break;
	case MU_CONFIG_CMD_SCRIPT:  merr = mu_cmd_script  (opts, err); break;
	case MU_CONFIG_CMD_VIEW:    merr = cmd_view       (opts, err); break;
	case MU_CONFIG_CMD_VERIFY:  merr = cmd_verify     (opts, err); break;
	case MU_CONFIG_CMD_EXTRACT: merr = mu_cmd_extract (opts, err); break;

	/* read-only store */

	case MU_CONFIG_CMD_CFIND:
		merr = with_readonly_store (mu_cmd_cfind, opts, err); break;
	case MU_CONFIG_CMD_FIND:
		merr = cmd_find(opts, err);  break;
	case MU_CONFIG_CMD_INFO:
		merr = with_readonly_store (cmd_info, opts,  err);  break;

	/* writable store */

	case MU_CONFIG_CMD_ADD:
		merr = with_writable_store (cmd_add, opts, err);      break;
	case MU_CONFIG_CMD_REMOVE:
		merr = with_writable_store (cmd_remove, opts, err);   break;
	case MU_CONFIG_CMD_TICKLE:
		merr = with_writable_store (cmd_tickle, opts, err);   break;
	case MU_CONFIG_CMD_INDEX:
		merr = with_writable_store (cmd_index, opts, err);   break;

	/* commands instantiate store themselves */
	case MU_CONFIG_CMD_INIT:
		merr = cmd_init (opts,err); break;
	case MU_CONFIG_CMD_SERVER:
		merr = mu_cmd_server (opts, err); break;

	default:
		merr = MU_ERROR_IN_PARAMETERS; break;
	}

	return merr;

} catch (const Mu::Error& er) {
        g_set_error(err, MU_ERROR_DOMAIN, MU_ERROR, "%s", er.what());
        return MU_ERROR;
} catch (...) {
        g_set_error(err, MU_ERROR_DOMAIN, MU_ERROR, "%s", "caught exception");
        return MU_ERROR;
}
