#ifndef TLEXC_HPP
#define TLEXC_HPP

#if defined(NDEBUG) || !defined(TLEXC_IGNORE_NDEBUG)
#warning [TLEXC]: Not in Debug Mode! TLEXC is not meant to be used in production!
#endif

#include <string>
#include <stdexcept>

/// Template Err with title
class titled_generic_error : public std::runtime_error {
public:
    virtual const char* title() const = 0;
protected:
    explicit titled_generic_error(const std::string& message)
        : std::runtime_error(message) {}
};

class init_error : public titled_generic_error {
public:
    explicit init_error(const std::string& message)
        : titled_generic_error(message) {}
    const char* title() const override { return "Init"; };
};

class mloop_error : public titled_generic_error {
public:
    explicit mloop_error(const std::string& message)
        : titled_generic_error(message) {}
    const char* title() const override { return "Main Loop"; };
};

class render_error : public titled_generic_error {
public:
    explicit render_error(const std::string& message)
        : titled_generic_error(message) {}
    const char* title() const override { return "Render"; };
};

class networking_error : public titled_generic_error {
public:
    explicit networking_error(const std::string& message)
        : titled_generic_error(message) {}
    const char* title() const override { return "Networking"; };
};

class sdl_error : public titled_generic_error {
public:
    explicit sdl_error(const std::string& message)
        : titled_generic_error(message) {}
    const char* title() const override { return "SDL"; };
};

class opengl_error : public titled_generic_error {
public:
    explicit opengl_error(const std::string& message)
        : titled_generic_error(message) {}
    const char* title() const override { return "OpenGL"; };
};

class dirctx_error : public titled_generic_error {
public:
    explicit dirctx_error(const std::string& message)
        : titled_generic_error(message) {}
    const char* title() const override { return "DirectX"; };
};

class vulkan_error : public titled_generic_error {
public:
    explicit vulkan_error(const std::string& message)
        : titled_generic_error(message) {}
    const char* title() const override { return "Vulkan"; };
};


/**
 * \brief Show an Error.
 * Calls either ShowErrGUI or ShowErrConsole
 * 
 * \param _err _err.what() as message
 * \param _title as title for the GUI Window or in the console before the err
 */
void ShowErr(
    const std::exception &_err,
    const char* const &_title = "None"
);

/**
 * \brief Show an Error.
 * Usally to a GUI, but if this fails show it to stderr.
 * 
 * \param _err _err.title() as title for the GUI Window or in the console before the err
 * \param _err _err.what() as message
 */
void ShowErr(
    const titled_generic_error &_err
);

/**
 * \brief Show an Error to the console.
 * 
 * \param _title as title for the GUI Window
 * \param _message as message
 */
void ShowErrConsole(
    const char* const &_title = "None",
    const char* const &_message = "None"
);

/**
 * \brief Show an Error to the console.
 * 
 * \param _title as title before the err in the console
 * \param _message as message
 */
int ShowErrGUI(
    const char* const &_title = "None",
    const char* const &_message = "None"
);

#endif /* TLEXC_HPP */



/* IMPLEMENTATION */
#if defined(TLEXC_IMPLEMENTATION) && !defined(_TLEXC_IMPLEMENTATION_GUARD)
#define _TLEXC_IMPLEMENTATION_GUARD

#include <sstream>
#include <stdio.h>

void ShowErrConsole(const char* const &_title, const char* const &_message) {
    printf("An error has occurred!\n[%s Error]: %s\n\n\nPress ENTER to continue...", _title, _message);
    getchar();
}

#if defined(TLEXC_GUI_NOT_IMPELEMENTET)
    // Implement the GUI yourself

#elif defined(TLEXC_GUI_CONSOLE)
    int ShowErrGUI(const char* const &_title, const char* const &_message) {
        ShowErrConsole(_title, _message);
        return 0;
    }

#elif defined(TLEXC_GUI_SDL2)
    #include <SDL2/SDL.h>

    int ShowErrGUI(const char* const &_title, const char* const &_message) {
        return SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, _title, _message, NULL);
    }

#elif defined(TLEXC_GUI_QT)
    #include <QMessageBox>

    int ShowErrGUI(const char* const &_title, const char* const &_message) {
        QMessageBox mb(QMessageBox::Critical, _title, _message, QMessageBox::Ok);
        mb.exec();
        return 0;
    }

#elif defined(_WIN32)
    #include <windows.h>

    int ShowErrGUI(const char* const &_title, const char* const &_message) {
        std::stringstream ss_title;
        ss_title << _title << " Error";

        // narrow character strings are good enough should only be English anyways
        MessageBoxA(NULL, _message, ss_title.str().c_str(), (MB_OK | MB_ICONERROR));
        return 0;
    }


#elif defined(__linux__) && !defined(TLEXC_GUI_NO_HACKY)
        #define L_ERRORBOX_COMMAND              "zenity"
        #define L_ERRORBOX_ARG_BEFORE_TITLE     " --error --title="
        #define L_ERRORBOX_ARG_BEFORE_TEXT      " --text="

        int ShowErrGUI(const char* const &_title, const char* const &_message) {
            // Not optimal: could use gtk or implement it yourself, but that is only to have something ._.
            std::stringstream ss_cmd;
            ss_cmd << L_ERRORBOX_COMMAND
               << L_ERRORBOX_ARG_BEFORE_TITLE << '\"' << _title << " Error\""
               << L_ERRORBOX_ARG_BEFORE_TEXT << '\"' << _message << '\"'
               << " > /dev/null 2>&1";

            // run the command, if return is 127 (not found) return 1
            if (system(ss_cmd.str().c_str()) == 127) {
                return 1;
            }
            return 0;
        }

#else
#warning [TLEXC]: No Error GUI (Console Only)
    int ShowErrGUI(const char* const &_title, const char* const &_message) {
        ShowErrConsole(_title, _message);
        return 0;
    }

#endif


void ShowErr(const std::exception &_err, const char* const &_title) {
    if (ShowErrGUI(_title, _err.what())) {
        ShowErrConsole(_title, _err.what());
    }
}

void ShowErr(const titled_generic_error &_err) {
    if (ShowErrGUI(_err.title(), _err.what())) {
        ShowErrConsole(_err.title(), _err.what());
    }
}

#endif /* TLEXC_IMPLEMENTATION && !_TLEXC_IMPLEMENTATION_GUARD */