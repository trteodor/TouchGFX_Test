/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#include <gui_generated/bmp280_screen/BMP280ViewBase.hpp>
#include <touchgfx/Color.hpp>
#include "BitmapDatabase.hpp"
#include <texts/TextKeysAndLanguages.hpp>

BMP280ViewBase::BMP280ViewBase() :
    buttonCallback(this, &BMP280ViewBase::buttonCallbackHandler)
{

    __background.setPosition(0, 0, 320, 240);
    __background.setColor(touchgfx::Color::getColorFrom24BitRGB(0, 0, 0));

    box1.setPosition(0, 0, 320, 240);
    box1.setColor(touchgfx::Color::getColorFrom24BitRGB(153, 145, 158));

    BackToMain.setXY(256, 0);
    BackToMain.setBitmaps(touchgfx::Bitmap(BITMAP_MENUSCALED_ID), touchgfx::Bitmap(BITMAP_MENUSCALEDWHITE_ID));
    BackToMain.setAction(buttonCallback);

    textArea1.setXY(120, 215);
    textArea1.setColor(touchgfx::Color::getColorFrom24BitRGB(0, 0, 0));
    textArea1.setLinespacing(0);
    textArea1.setTypedText(touchgfx::TypedText(T_SINGLEUSEID4));

    temperatureBar.setXY(28, 97);
    temperatureBar.setProgressIndicatorPosition(17, 27, 150, 18);
    temperatureBar.setRange(0, 90);
    temperatureBar.setDirection(touchgfx::AbstractDirectionProgress::RIGHT);
    temperatureBar.setBackground(touchgfx::Bitmap(BITMAP_HOR_THERM_BG_SCALE_ID));
    temperatureBar.setBitmap(BITMAP_HOR_THERM_PROGRESS_ID);
    temperatureBar.setValue(59);
    temperatureBar.setAnchorAtZero(true);

    add(__background);
    add(box1);
    add(BackToMain);
    add(textArea1);
    add(temperatureBar);
}

void BMP280ViewBase::setupScreen()
{

}

void BMP280ViewBase::buttonCallbackHandler(const touchgfx::AbstractButton& src)
{
    if (&src == &BackToMain)
    {
        //Interaction1
        //When BackToMain clicked change screen to MainScreen
        //Go to MainScreen with no screen transition
        application().gotoMainScreenScreenNoTransition();
    }
}
