/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#ifndef LEDSCRVIEWBASE_HPP
#define LEDSCRVIEWBASE_HPP

#include <gui/common/FrontendApplication.hpp>
#include <mvp/View.hpp>
#include <gui/ledscr_screen/LedScrPresenter.hpp>
#include <touchgfx/widgets/Box.hpp>
#include <touchgfx/widgets/Button.hpp>
#include <touchgfx/widgets/TextArea.hpp>
#include <touchgfx/widgets/Image.hpp>
#include <touchgfx/widgets/ToggleButton.hpp>

class LedScrViewBase : public touchgfx::View<LedScrPresenter>
{
public:
    LedScrViewBase();
    virtual ~LedScrViewBase() {}
    virtual void setupScreen();

    /*
     * Virtual Action Handlers
     */
    virtual void Led1bToggle()
    {
        // Override and implement this function in LedScr
    }

    virtual void Led2bToggle()
    {
        // Override and implement this function in LedScr
    }

protected:
    FrontendApplication& application() {
        return *static_cast<FrontendApplication*>(touchgfx::Application::getInstance());
    }

    /*
     * Member Declarations
     */
    touchgfx::Box __background;
    touchgfx::Box box1;
    touchgfx::Button MainMenu;
    touchgfx::TextArea textArea1;
    touchgfx::TextArea textArea1_1;
    touchgfx::Image Pressed;
    touchgfx::Image dontpressed;
    touchgfx::ToggleButton led1b;
    touchgfx::ToggleButton led2b;

private:

    /*
     * Callback Declarations
     */
    touchgfx::Callback<LedScrViewBase, const touchgfx::AbstractButton&> buttonCallback;

    /*
     * Callback Handler Declarations
     */
    void buttonCallbackHandler(const touchgfx::AbstractButton& src);

};

#endif // LEDSCRVIEWBASE_HPP
