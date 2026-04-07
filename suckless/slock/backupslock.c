/* See LICENSE file for license details. */
#define _XOPEN_SOURCE   500
#define LENGTH(X)       (sizeof X / sizeof X[0])
#if HAVE_SHADOW_H
#include <shadow.h>
#endif

#include <ctype.h>
#include <errno.h>
#include <grp.h>
#include <pwd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <spawn.h>
#include <sys/types.h>
#include <X11/extensions/Xrandr.h>
#ifdef XINERAMA
#include <X11/extensions/Xinerama.h>
#endif
#include <X11/keysym.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xft/Xft.h>
#include <security/pam_appl.h>
#include "arg.h"
#include "util.h"

char *argv0;
static char passwd[256];

enum {
    BACKGROUND,
    INIT,
    INPUT,
    FAILED,
    PAM,
    NUMCOLS
};

#include "config.h"

struct lock {
    int screen;
    Window root, win;
    Pixmap pmap;
    unsigned long colors[NUMCOLS];
    unsigned int x, y;
    unsigned int xoff, yoff, mw, mh;
    Drawable drawable;
    GC gc;
    XRectangle rectangles[LENGTH(rectangles)];
};

struct xrandr {
    int active;
    int evbase;
    int errbase;
};

static void
die(const char *errstr, ...)
{
    va_list ap;

    va_start(ap, errstr);
    vfprintf(stderr, errstr, ap);
    va_end(ap);
    exit(1);
}

static int
pam_conv(int num_msg, const struct pam_message **msg,
        struct pam_response **resp, void *appdata_ptr)
{
    int retval = PAM_CONV_ERR;
    for(int i=0; i<num_msg; i++) {
        if (msg[i]->msg_style == PAM_PROMPT_ECHO_OFF &&
                strncmp(msg[i]->msg, "Password: ", 10) == 0) {
            struct pam_response *resp_msg = malloc(sizeof(struct pam_response));
            if (!resp_msg)
                die("malloc failed\n");
            char *password = malloc(strlen(passwd) + 1);
            if (!password)
                die("malloc failed\n");
            memset(password, 0, strlen(passwd) + 1);
            strcpy(password, passwd);
            resp_msg->resp_retcode = 0;
            resp_msg->resp = password;
            resp[i] = resp_msg;
            retval = PAM_SUCCESS;
        }
    }
    return retval;
}

#ifdef __linux__
#include <fcntl.h>
#include <linux/oom.h>

static void
dontkillme(void)
{
    FILE *f;
    const char oomfile[] = "/proc/self/oom_score_adj";

    if (!(f = fopen(oomfile, "w"))) {
        if (errno == ENOENT)
            return;
        die("slock: fopen %s: %s\n", oomfile, strerror(errno));
    }
    fprintf(f, "%d", OOM_SCORE_ADJ_MIN);
    if (fclose(f)) {
        if (errno == EACCES)
            die("slock: unable to disable OOM killer. "
                "Make sure to suid or sgid slock.\n");
        else
            die("slock: fclose %s: %s\n", oomfile, strerror(errno));
    }
}
#endif

static const char *
gethash(void)
{
    const char *hash;
    struct passwd *pw;

    errno = 0;
    if (!(pw = getpwuid(getuid()))) {
        if (errno)
            die("slock: getpwuid: %s\n", strerror(errno));
        else
            die("slock: cannot retrieve password entry\n");
    }
    hash = pw->pw_passwd;

#if HAVE_SHADOW_H
    if (!strcmp(hash, "x")) {
        struct spwd *sp;
        if (!(sp = getspnam(pw->pw_name)))
            die("slock: getspnam: cannot retrieve shadow entry. "
                "Make sure to suid or sgid slock.\n");
        hash = sp->sp_pwdp;
    }
#else
    if (!strcmp(hash, "*")) {
#ifdef __OpenBSD__
        if (!(pw = getpwuid_shadow(getuid())))
            die("slock: getpwnam_shadow: cannot retrieve shadow entry. "
                "Make sure to suid or sgid slock.\n");
        hash = pw->pw_passwd;
#else
        die("slock: getpwuid: cannot retrieve shadow entry. "
            "Make sure to suid or sgid slock.\n");
#endif /* __OpenBSD__ */
    }
#endif /* HAVE_SHADOW_H */

    hash = pw->pw_name;
    return hash;
}

static void
resizerectangles(struct lock *lock)
{
    int i;

    for (i = 0; i < LENGTH(rectangles); i++){
        lock->rectangles[i].x = (rectangles[i].x * logosize)
                                + lock->xoff + ((lock->mw) / 2) - (logow / 2 * logosize);
        lock->rectangles[i].y = (rectangles[i].y * logosize)
                                + lock->yoff + ((lock->mh) / 2) - (logoh / 2 * logosize);
        lock->rectangles[i].width = rectangles[i].width * logosize;
        lock->rectangles[i].height = rectangles[i].height * logosize;
    }
}

static void
drawlogo(Display *dpy, struct lock *lock, int color)
{
    XSetForeground(dpy, lock->gc, lock->colors[BACKGROUND]);
    XFillRectangle(dpy, lock->drawable, lock->gc, 0, 0, lock->x, lock->y);
    XSetForeground(dpy, lock->gc, lock->colors[color]);
    XFillRectangles(dpy, lock->drawable, lock->gc, lock->rectangles, LENGTH(rectangles));
    XCopyArea(dpy, lock->drawable, lock->win, lock->gc, 0, 0, lock->x, lock->y, 0, 0);
    XSync(dpy, False);
}

static void
readpw(Display *dpy, struct xrandr *rr, struct lock **locks, int nscreens,
       const char *hash)
{
    XRRScreenChangeNotifyEvent *rre;
    char buf[32];
    int num, screen, running, failure, oldc, retval;
    unsigned int len, color;
    KeySym ksym;
    XEvent ev;
    pam_handle_t *pamh;
    struct pam_conv pamc = { pam_conv, NULL };

    len = 0;
    running = 1;
    failure = 0;
    oldc = INIT;
//  if (pam_start(pam_service, hash, &pamc, &pamh) == PAM_SUCCESS) {
        //if (pam_authenticate(pamh, PAM_SILENT) == PAM_SUCCESS) {
           // running = 0;
    //    }
     //   pam_end(pamh, PAM_SUCCESS);
    //}


    while (running && !XNextEvent(dpy, &ev)) {
        if (ev.type == KeyPress) {
            explicit_bzero(&buf, sizeof(buf));
            num = XLookupString(&ev.xkey, buf, sizeof(buf), &ksym, 0);
            if (IsKeypadKey(ksym)) {
                if (ksym == XK_KP_Enter)
                    ksym = XK_Return;
                else if (ksym >= XK_KP_0 && ksym <= XK_KP_9)
                    ksym = (ksym - XK_KP_0) + XK_0;
            }
            if (IsFunctionKey(ksym) ||
                IsKeypadKey(ksym) ||
                IsMiscFunctionKey(ksym) ||
                IsPFKey(ksym) ||
                IsPrivateKeypadKey(ksym))
                continue;
            switch (ksym) {
                case XK_Return:
                passwd[len] = '\0';
                errno = 0;
                
                /* Visual feedback: Blue means "I'm checking something" */
                color = INPUT; 
                for (screen = 0; screen < nscreens; screen++) {
                    drawlogo(dpy, locks[screen], color);
                }
                XSync(dpy, False);

                retval = pam_start(pam_service, hash, &pamc, &pamh);
                if (retval == PAM_SUCCESS) {
                    /* PAM will now run the stack we defined in /etc/pam.d/slock */
                    /* If passwd is empty, phone auth usually triggers. */
                    /* If passwd has text, phone auth fails and it hits system-auth. */
                    retval = pam_authenticate(pamh, 0);
                    if (retval == PAM_SUCCESS)
                        retval = pam_acct_mgmt(pamh, 0);
                }

                if (retval == PAM_SUCCESS) {
                    running = 0;
                } else {
                    /* If both phone AND password fail, turn RED */
                    XBell(dpy, 100);
                    failure = 1;
                    color = FAILED;
                    for (screen = 0; screen < nscreens; screen++) {
                        drawlogo(dpy, locks[screen], color);
                    }
                }
                
                pam_end(pamh, retval);
                explicit_bzero(&passwd, sizeof(passwd));
                len = 0;
                break;            case XK_Escape:
                explicit_bzero(&passwd, sizeof(passwd));
                len = 0;
                break;
            case XK_BackSpace:
                if (len)
                    passwd[--len] = '\0';
                break;
            default:
                if (num && !iscntrl((int)buf[0]) &&
                    (len + num < sizeof(passwd))) {
                    memcpy(passwd + len, buf, num);
                    len += num;
                }
                break;
            }
            color = len ? INPUT : ((failure || failonclear) ? FAILED : INIT);
            if (running && oldc != color) {
                for (screen = 0; screen < nscreens; screen++) {
                    drawlogo(dpy, locks[screen], color);
                }
                oldc = color;
            }
        } else if (rr->active && ev.type == rr->evbase + RRScreenChangeNotify) {
            rre = (XRRScreenChangeNotifyEvent*)&ev;
            for (screen = 0; screen < nscreens; screen++) {
                if (locks[screen]->win == rre->window) {
                    if (rre->rotation == RR_Rotate_90 ||
                        rre->rotation == RR_Rotate_270)
                        XResizeWindow(dpy, locks[screen]->win,
                                      rre->height, rre->width);
                    else
                        XResizeWindow(dpy, locks[screen]->win,
                                      rre->width, rre->height);
                    XClearWindow(dpy, locks[screen]->win);
                    break;
                }
            }
        } else {
            for (screen = 0; screen < nscreens; screen++)
                XRaiseWindow(dpy, locks[screen]->win);
        }
    }
}

static struct lock *
lockscreen(Display *dpy, struct xrandr *rr, int screen)
{
    char curs[] = {0, 0, 0, 0, 0, 0, 0, 0};
    int i, ptgrab, kbgrab;
    struct lock *lock;
    XColor color, dummy;
    XSetWindowAttributes wa;
    Cursor invisible;
#ifdef XINERAMA
    XineramaScreenInfo *info;
    int n;
#endif

    if (dpy == NULL || screen < 0 || !(lock = malloc(sizeof(struct lock))))
        return NULL;

    lock->screen = screen;
    lock->root = RootWindow(dpy, lock->screen);

    for (i = 0; i < NUMCOLS; i++) {
        XAllocNamedColor(dpy, DefaultColormap(dpy, lock->screen),
                         colorname[i], &color, &dummy);
        lock->colors[i] = color.pixel;
    }

    lock->x = DisplayWidth(dpy, lock->screen);
    lock->y = DisplayHeight(dpy, lock->screen);
#ifdef XINERAMA
    if ((info = XineramaQueryScreens(dpy, &n))) {
        lock->xoff = info[0].x_org;
        lock->yoff = info[0].y_org;
        lock->mw = info[0].width;
        lock->mh = info[0].height;
    } else
#endif
    {
        lock->xoff = lock->yoff = 0;
        lock->mw = lock->x;
        lock->mh = lock->y;
    }
    lock->drawable = XCreatePixmap(dpy, lock->root,
            lock->x, lock->y, DefaultDepth(dpy, screen));
    lock->gc = XCreateGC(dpy, lock->root, 0, NULL);
    XSetLineAttributes(dpy, lock->gc, 1, LineSolid, CapButt, JoinMiter);

    wa.override_redirect = 1;
    wa.background_pixel = lock->colors[BACKGROUND];
    lock->win = XCreateWindow(dpy, lock->root, 0, 0,
                              lock->x, lock->y,
                              0, DefaultDepth(dpy, lock->screen),
                              CopyFromParent,
                              DefaultVisual(dpy, lock->screen),
                              CWOverrideRedirect | CWBackPixel, &wa);
    lock->pmap = XCreateBitmapFromData(dpy, lock->win, curs, 8, 8);
    invisible = XCreatePixmapCursor(dpy, lock->pmap, lock->pmap,
                                    &color, &color, 0, 0);
    XDefineCursor(dpy, lock->win, invisible);

    resizerectangles(lock);

    for (i = 0, ptgrab = kbgrab = -1; i < 6; i++) {
        if (ptgrab != GrabSuccess) {
            ptgrab = XGrabPointer(dpy, lock->root, False,
                                  ButtonPressMask | ButtonReleaseMask |
                                  PointerMotionMask, GrabModeAsync,
                                  GrabModeAsync, None, invisible, CurrentTime);
        }
        if (kbgrab != GrabSuccess) {
            kbgrab = XGrabKeyboard(dpy, lock->root, True,
                                   GrabModeAsync, GrabModeAsync, CurrentTime);
        }

        if (ptgrab == GrabSuccess && kbgrab == GrabSuccess) {
            XMapRaised(dpy, lock->win);
            if (rr->active)
                XRRSelectInput(dpy, lock->win, RRScreenChangeNotifyMask);

            XSelectInput(dpy, lock->root, SubstructureNotifyMask);
            drawlogo(dpy, lock, INIT);
            return lock;
        }

        if ((ptgrab != AlreadyGrabbed && ptgrab != GrabSuccess) ||
            (kbgrab != AlreadyGrabbed && kbgrab != GrabSuccess))
            break;

        usleep(100000);
    }

    if (ptgrab != GrabSuccess)
        fprintf(stderr, "slock: unable to grab mouse pointer for screen %d\n",
                screen);
    if (kbgrab != GrabSuccess)
        fprintf(stderr, "slock: unable to grab keyboard for screen %d\n",
                screen);
    return NULL;
}

static void
usage(void)
{
    die("usage: slock [-v] [cmd [arg ...]]\n");
}

int
main(int argc, char **argv) {
    struct xrandr rr;
    struct lock **locks;
    struct passwd *pwd;
    struct group *grp;
    uid_t duid;
    gid_t dgid;
    const char *hash;
    Display *dpy;
    int s, nlocks, nscreens;

    ARGBEGIN {
    case 'v':
        puts("slock-"VERSION);
        return 0;
    default:
        usage();
    } ARGEND

    errno = 0;
    if (!(pwd = getpwnam(user)))
        die("slock: getpwnam %s: %s\n", user,
            errno ? strerror(errno) : "user entry not found");
    duid = pwd->pw_uid;
    errno = 0;
    if (!(grp = getgrnam(group)))
        die("slock: getgrnam %s: %s\n", group,
            errno ? strerror(errno) : "group entry not found");
    dgid = grp->gr_gid;

#ifdef __linux__
    dontkillme();
#endif

    hash = gethash();
    errno = 0;

    if (!(dpy = XOpenDisplay(NULL)))
        die("slock: cannot open display\n");

    if (setgroups(0, NULL) < 0)
        die("slock: setgroups: %s\n", strerror(errno));
    if (setgid(dgid) < 0)
        die("slock: setgid: %s\n", strerror(errno));
    if (setuid(duid) < 0)
        die("slock: setuid: %s\n", strerror(errno));

    rr.active = XRRQueryExtension(dpy, &rr.evbase, &rr.errbase);

    nscreens = ScreenCount(dpy);
    if (!(locks = calloc(nscreens, sizeof(struct lock *))))
        die("slock: out of memory\n");
    for (nlocks = 0, s = 0; s < nscreens; s++) {
        if ((locks[s] = lockscreen(dpy, &rr, s)) != NULL)
            nlocks++;
        else
            break;
    }
    XSync(dpy, 0);

    if (nlocks != nscreens)
        return 1;

    if (argc > 0) {
        pid_t pid;
        extern char **environ;
        int err = posix_spawnp(&pid, argv[0], NULL, NULL, argv, environ);
        if (err) {
            die("slock: failed to execute post-lock command: %s: %s\n",
                argv[0], strerror(err));
        }
    }

    readpw(dpy, &rr, locks, nscreens, hash);

    for (nlocks = 0, s = 0; s < nscreens; s++) {
        XFreePixmap(dpy, locks[s]->drawable);
        XFreeGC(dpy, locks[s]->gc);
    }

    XSync(dpy, 0);
    XCloseDisplay(dpy);
    return 0;
}
