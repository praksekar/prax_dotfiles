This theme is designed for colour-contrast accessibility.

1. Provide a consistent minimum contrast ratio between background and
foreground values of 7:1 or higher.  This meets the highest such
accessibility criterion per the guidelines of the Worldwide Web
Consortium's Working Group on Accessibility (WCAG AAA standard).

2. Offer as close to full face coverage as possible.  The list is
already quite long (see further below), with more additions to follow
as part of the ongoing development process.

The theme provides the following customisation options, all of which
are disabled by default:

    modus-vivendi-theme-slanted-constructs             (boolean)
    modus-vivendi-theme-bold-constructs                (boolean)
    modus-vivendi-theme-variable-pitch-headings        (boolean)
    modus-vivendi-theme-rainbow-headings               (boolean)
    modus-vivendi-theme-section-headings               (boolean)
    modus-vivendi-theme-scale-headings                 (boolean)
    modus-vivendi-theme-visible-fringes                (boolean)
    modus-vivendi-theme-org-blocks                     (choice)
    modus-vivendi-theme-3d-modeline                    (boolean)
    modus-vivendi-theme-subtle-diffs                   (boolean)
    modus-vivendi-theme-faint-syntax                   (boolean)
    modus-vivendi-theme-intense-hl-line                (boolean)
    modus-vivendi-theme-intense-paren-match            (boolean)
    modus-vivendi-theme-intense-standard-completions   (boolean)
    modus-vivendi-theme-override-colors-alist          (alist)

The default scale is as follows (it can be customised as well):

    modus-vivendi-theme-scale-1 1.05
    modus-vivendi-theme-scale-2 1.1
    modus-vivendi-theme-scale-3 1.15
    modus-vivendi-theme-scale-4 1.2
    modus-vivendi-theme-scale-5 1.3

What follows is the list of explicitly supported packages or face
groups (there are implicitly supported packages as well, which
inherit from font-lock or some basic group).  You are encouraged to
notify me of any missing package or change you would like to see.

    ace-window
    ag
    alert
    all-the-icons
    annotate
    anzu
    apropos
    apt-sources-list
    artbollocks-mode
    auctex and TeX
    auto-dim-other-buffers
    avy
    bm
    bongo
    boon
    breakpoint (provided by built-in gdb-mi.el)
    buffer-expose
    calendar and diary
    calfw
    centaur-tabs
    change-log and log-view (`vc-print-log' and `vc-print-root-log')
    cider
    circe
    color-rg
    column-enforce-mode
    company-mode
    company-posframe
    compilation-mode
    completions
    counsel
    counsel-css
    counsel-notmuch
    counsel-org-capture-string
    cov
    csv-mode
    ctrlf
    custom (M-x customize)
    dap-mode
    dashboard (emacs-dashboard)
    deadgrep
    debbugs
    define-word
    deft
    dictionary
    diff-hl
    diff-mode
    dim-autoload
    dired
    dired-async
    dired-git
    dired-git-info
    dired-narrow
    dired-subtree
    diredfl
    disk-usage
    doom-modeline
    dynamic-ruler
    easy-jekyll
    easy-kill
    ebdb
    ediff
    eglot
    el-search
    eldoc-box
    elfeed
    elfeed-score
    emms
    enhanced-ruby-mode
    epa
    equake
    erc
    eros
    ert
    eshell
    eshell-fringe-status
    eshell-git-prompt
    eshell-prompt-extras (epe)
    evil (evil-mode)
    evil-goggles
    evil-visual-mark-mode
    eww
    eyebrowse
    fancy-dabbrev
    flycheck
    flycheck-indicator
    flycheck-posframe
    flymake
    flyspell
    flyspell-correct
    flx
    freeze-it
    frog-menu
    focus
    fold-this
    font-lock (generic syntax highlighting)
    forge
    fountain (fountain-mode)
    geiser
    git
    git-gutter (and variants)
    git-lens
    git-timemachine
    git-walktree
    gnus
    golden-ratio-scroll-screen
    helm
    helm-ls-git
    helm-switch-shell
    helm-xref
    helpful
    highlight-blocks
    highlight-defined
    highlight-escape-sequences (`hes-mode')
    highlight-indentation
    highlight-numbers
    highlight-symbol
    highlight-tail
    highlight-thing
    hl-defined
    hl-fill-column
    hl-line-mode
    hl-todo
    hydra
    hyperlist
    ibuffer
    icomplete
    ido-mode
    iedit
    iflipb
    imenu-list
    indium
    info
    info-colors
    interaction-log
    ioccur
    isearch, occur, etc.
    ivy
    ivy-posframe
    jira (org-jira)
    journalctl-mode
    js2-mode
    julia
    jupyter
    kaocha-runner
    keycast
    line numbers (`display-line-numbers-mode' and global variant)
    lsp-mode
    lsp-ui
    magit
    magit-imerge
    man
    markdown-mode
    markup-faces (`adoc-mode')
    mentor
    messages
    minibuffer-line
    minimap
    modeline
    mood-line
    mu4e
    mu4e-conversation
    multiple-cursors
    neotree
    no-emoji
    notmuch
    num3-mode
    nxml-mode
    orderless
    org
    org-journal
    org-noter
    org-pomodoro
    org-recur
    org-roam
    org-superstar
    org-treescope
    origami
    outline-mode
    outline-minor-faces
    package (M-x list-packages)
    page-break-lines
    paradox
    paren-face
    parrot
    pass
    persp-mode
    perspective
    phi-grep
    phi-search
    pomidor
    powerline
    powerline-evil
    proced
    prodigy
    rainbow-blocks
    rainbow-identifiers
    rainbow-delimiters
    rcirc
    regexp-builder (also known as `re-builder')
    rg
    ripgrep
    rmail
    ruler-mode
    sallet
    selectrum
    sesman
    shell-script-mode
    show-paren-mode
    side-notes
    skewer-mode
    smart-mode-line
    smartparens
    smerge
    speedbar
    spell-fu
    stripes
    suggest
    switch-window
    swiper
    swoop
    sx
    symbol-overlay
    tab-bar-mode
    tab-line-mode
    syslog-mode
    trashed
    telephone-line
    term
    tomatinho
    transient (pop-up windows like Magit's)
    treemacs
    tty-menu
    tuareg
    undo-tree
    vc (built-in mode line status for version control)
    vc-annotate (C-x v g)
    vdiff
    vimish-fold
    visible-mark
    visual-regexp
    volatile-highlights
    vterm
    wcheck-mode
    web-mode
    wgrep
    which-function-mode
    which-key
    whitespace-mode
    window-divider-mode
    winum
    writegood-mode
    woman
    xah-elisp-mode
    xref
    xterm-color (and ansi-colors)
    yaml-mode
    yasnippet
    ztree
