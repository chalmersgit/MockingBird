#define GL_GLEXT_PROTOTYPES
#define GLX_GLXEXT_PROTOTYPES


#include <stdio.h>
#include <iostream>

#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>

#include "GL/glxext.h"
#include "GL/glext.h"


// -------------------------------------------------------- //
// Globals
// -------------------------------------------------------- //

Display *d_dpy;
Window d_win;
GLXContext d_ctx;

const char *display = ":0";
int width = 800;
int height = 800;

// -------------------------------------------------------- //


int main(void) {

    if (!(d_dpy = XOpenDisplay(display))) {
        throw std::string("Couldn't open X11 display\n");
    }

    int attr[] = {
        GLX_RGBA,
        GLX_RED_SIZE, 1,
        GLX_GREEN_SIZE, 1,
        GLX_BLUE_SIZE, 1,
        GLX_DOUBLEBUFFER,
        GLX_DEPTH_SIZE, 1,
        None
    };

    int scrnum = DefaultScreen(d_dpy);
    Window root = RootWindow(d_dpy, scrnum);

    int elemc;
    GLXFBConfig *fbcfg = glXChooseFBConfig(d_dpy, scrnum, NULL, &elemc);
    if (!fbcfg) {
        throw std::string ("Couldn't get FB configs\n");
    } else {
        printf("Got %d FB configs\n", elemc);
    }

    XVisualInfo *visinfo = glXChooseVisual(d_dpy, scrnum, attr);
    if(!visinfo) {
        throw std::string("Couldn't get a visual\n");
    }

    // Window Parameters
    XSetWindowAttributes winattr;
    winattr.background_pixel = 0;
    winattr.border_pixel = 0;
    winattr.colormap = XCreateColormap(d_dpy, root, visinfo->visual, AllocNone);
    winattr.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask;
    unsigned long mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;

    printf("Window depth %d, w %d h %d\n", visinfo->depth, width, height);
    d_win = XCreateWindow(d_dpy, root, 0, 0, width, height, 0, visinfo->depth, InputOutput, visinfo->visual, mask, &winattr);

    // Opengl
    int gl3attr[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
        GLX_CONTEXT_MINOR_VERSION_ARB, 2,
        GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        None
    };

    d_ctx = glXCreateContextAttribsARB(d_dpy, fbcfg[0], NULL, true, gl3attr);

    XFree(visinfo);
    XMapWindow(d_dpy, d_win);
    glXMakeCurrent(d_dpy, d_win, d_ctx);

    printf("OpenGL:\n\tvendor %s\nrender %s\n\tversion %s\n\tsahder language%s\n\n",
            glGetString(GL_VENDOR), glGetString(GL_RENDERER), glGetString(GL_VERSION),
            glGetString(GL_SHADING_LANGUAGE_VERSION));

    int extCount;
    glGetIntegerv(GL_NUM_EXTENSIONS, &extCount);
    for (int i = 0; i < extCount; ++i) {
        //printf("Extension %d/%d: %s\n", i + 1, extCount, glGetStringi(GL_EXTENSIONS, i));
    }

    glViewport(0, 0, width, height);

    std::cin.ignore();
    return 0;
}


