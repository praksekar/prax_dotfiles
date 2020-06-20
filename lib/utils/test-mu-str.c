/* -*-mode: c; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*-*/

/*
** Copyright (C) 2008-2013 Dirk-Jan C. Binnema <djcb@djcbsoftware.nl>
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

#if HAVE_CONFIG_H
#include "config.h"
#endif /*HAVE_CONFIG_H*/

#include <glib.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include <locale.h>

#include "mu-str.h"


static void
test_mu_str_size_01 (void)
{
	struct lconv *lc;
	char *tmp2;

	lc = localeconv();

	g_assert_cmpstr (mu_str_size_s (0), ==, "0 bytes");

	tmp2 = g_strdup_printf ("97%s7 KB", lc->decimal_point);
	g_assert_cmpstr (mu_str_size_s (100000), ==, tmp2);
	g_free (tmp2);

	tmp2 = g_strdup_printf ("1%s0 MB", lc->decimal_point);
	g_assert_cmpstr (mu_str_size_s (1100*1000), ==,  tmp2);
	g_free (tmp2);
}



static void
test_mu_str_size_02 (void)
{
	struct lconv *lc;
	char *tmp1, *tmp2;

	lc = localeconv();

	tmp2 = g_strdup_printf ("1%s0 MB", lc->decimal_point);
	tmp1 = mu_str_size (999999);
	g_assert_cmpstr (tmp1, !=, tmp2);

	g_free (tmp1);
	g_free (tmp2);
}

static void
test_mu_str_esc_to_list (void)
{
	int			i;
	struct {
		const char*  str;
		const char* strs[3];
	} strings [] = {
		{ "maildir:foo",
		  {"maildir:foo", NULL, NULL}},
		{ "maildir:sent items",
		  {"maildir:sent", "items", NULL}},
		{ "\"maildir:sent items\"",
		  {"maildir:sent items", NULL, NULL}},
	};

	for (i = 0; i != G_N_ELEMENTS(strings); ++i) {
		GSList *lst, *cur;
		unsigned u;
		lst = mu_str_esc_to_list (strings[i].str);
		for (cur = lst, u = 0; cur; cur = g_slist_next(cur), ++u)
			g_assert_cmpstr ((const char*)cur->data,==,
					 strings[i].strs[u]);
		mu_str_free_list (lst);
	}
}



static void
assert_cmplst (GSList *lst, const char *items[])
{
	int i;

	if (!lst)
		g_assert (!items);

	for (i = 0; lst; lst = g_slist_next(lst), ++i)
		g_assert_cmpstr ((char*)lst->data,==,items[i]);

	g_assert (items[i] == NULL);
}


static GSList*
create_list (const char *items[])
{
	GSList *lst;

	lst = NULL;
	while (items && *items) {
		lst = g_slist_prepend (lst, g_strdup(*items));
		++items;
	}

	return g_slist_reverse (lst);

}

static void
test_mu_str_from_list (void)
{
	{
		const char *strs[] = {"aap", "noot", "mies", NULL};
		GSList *lst = create_list (strs);
		gchar  *str = mu_str_from_list (lst, ',');
		g_assert_cmpstr ("aap,noot,mies", ==, str);
		mu_str_free_list (lst);
		g_free (str);
	}

	{
		const char *strs[] = {"aap", "no,ot", "mies", NULL};
		GSList *lst = create_list (strs);
		gchar  *str = mu_str_from_list (lst, ',');
		g_assert_cmpstr ("aap,no,ot,mies", ==, str);
		mu_str_free_list (lst);
		g_free (str);
	}

	{
		const char *strs[] = {NULL};
		GSList *lst = create_list (strs);
		gchar  *str = mu_str_from_list (lst,'@');
		g_assert_cmpstr (NULL, ==, str);
		mu_str_free_list (lst);
		g_free (str);
	}


}


static void
test_mu_str_to_list (void)
{
	{
		const char *items[]= {"foo", "bar ", "cuux", NULL};
		GSList *lst = mu_str_to_list ("foo@bar @cuux",'@', FALSE);
		assert_cmplst (lst, items);
		mu_str_free_list (lst);
	}

	{
		GSList *lst = mu_str_to_list (NULL,'x',FALSE);
		g_assert (lst == NULL);
		mu_str_free_list (lst);
	}
}

static void
test_mu_str_to_list_strip (void)
{
	const char *items[]= {"foo", "bar", "cuux", NULL};
	GSList *lst = mu_str_to_list ("foo@bar @cuux",'@', TRUE);
	assert_cmplst (lst, items);
		mu_str_free_list (lst);
}


static void
test_mu_str_replace (void)
{
	unsigned u;
	struct {
		const char*  str;
		const char* sub;
		const char *repl;
		const char *exp;
	} strings [] = {
		{ "hello", "ll", "xx", "hexxo" },
		{ "hello", "hello", "hi", "hi" },
		{ "hello", "foo", "bar", "hello" }
	};

	for (u = 0; u != G_N_ELEMENTS(strings); ++u) {
		char *res;
		res = mu_str_replace (strings[u].str,
				      strings[u].sub,
				      strings[u].repl);
		g_assert_cmpstr (res,==,strings[u].exp);
		g_free (res);
	}
}


static void
test_mu_str_remove_ctrl_in_place (void)
{
	unsigned u;
	struct {
		char *str;
		const char *exp;
	} strings [] = {
		{ g_strdup(""), ""},
		{ g_strdup("hello, world!"), "hello, world!" },
		{ g_strdup("hello,\tworld!"), "hello, world!" },
		{ g_strdup("hello,\n\nworld!"), "hello,  world!", },
		{ g_strdup("hello,\x1f\x1e\x1ew\nor\nld!"), "hello,w or ld!" },
		{ g_strdup("\x1ehello, world!\x1f"), "hello, world!" }
	};

	for (u = 0; u != G_N_ELEMENTS(strings); ++u) {
		char *res;
		res = mu_str_remove_ctrl_in_place (strings[u].str);
		g_assert_cmpstr (res,==,strings[u].exp);
		g_free (strings[u].str);
	}
}


int
main (int argc, char *argv[])
{
	setlocale (LC_ALL, "");

	g_test_init (&argc, &argv, NULL);

	/* mu_str_size */
	g_test_add_func ("/mu-str/mu-str-size-01",
			 test_mu_str_size_01);
	g_test_add_func ("/mu-str/mu-str-size-02",
			 test_mu_str_size_02);

	g_test_add_func ("/mu-str/mu-str-from-list",
			 test_mu_str_from_list);
	g_test_add_func ("/mu-str/mu-str-to-list",
			 test_mu_str_to_list);
	g_test_add_func ("/mu-str/mu-str-to-list-strip",
			 test_mu_str_to_list_strip);

	g_test_add_func ("/mu-str/mu-str-replace",
			 test_mu_str_replace);

	g_test_add_func ("/mu-str/mu-str-esc-to-list",
			 test_mu_str_esc_to_list);

	g_test_add_func ("/mu-str/mu_str_remove_ctrl_in_place",
			 test_mu_str_remove_ctrl_in_place);


	return g_test_run ();
}
