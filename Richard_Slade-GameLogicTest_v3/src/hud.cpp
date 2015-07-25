#include <string>

#include "App/HUD.hpp"
#include "World/World.hpp"

HUD::HUD(World* world
         , const sf::Font& font
         , sf::FloatRect viewBounds
         , std::string username)
: mWorld(world)
, mHUDBackground(sf::Vector2f(viewBounds.width
                              , viewBounds.height / 16.f))
{
    mHUDBackground.setFillColor(sf::Color::Black);

    for(size_t i = 0; i < HUD::DataType::NumDataType; i++)
        mHUDText.push_back(sf::Text("", font));

    mHUDText.at(HUD::DataType::Name).setString(username);
}

void HUD::draw(sf::RenderTarget& target
                      , sf::RenderStates states) const
{
    target.draw(mHUDBackground);

    target.draw(mHUDText.at(HUD::DataType::Name));
    target.draw(mHUDText.at(HUD::DataType::Enemy));
    target.draw(mHUDText.at(HUD::DataType::Lives));
}

void HUD::update(int playerLives)
{
    mHUDText.at(HUD::DataType::Enemy).setString("");

//    int time = static_cast<int>(mWorld->getTimeLeft().asSeconds());
    //int time = static_cast<int>(0.f);
    //int mins = time / 60;
    //int secs = time % 60;

    //std::string minStr("0"
    //                   + std::to_string(mins));

    //std::string secStr;

    //if(secs < 10)
    //    secStr = "0";

    //secStr += std::to_string(secs);

    //std::string timeStr(minStr
    //                    + ":"
    //                    + secStr);


  mHUDText.at(HUD::DataType::Lives).setString("Lives: "
                                              + std::to_string(playerLives));

  for(sf::Text& txt : mHUDText)
  {
      sf::FloatRect bounds = txt.getLocalBounds();
      txt.setOrigin(bounds.width / 2.f, 0.f);
  }
}

void HUD::setHUDPosition(sf::FloatRect viewBounds)
{
    mHUDBackground.setPosition(viewBounds.left
                               , viewBounds.top);

    mHUDText.at(HUD::DataType::Name).setPosition(viewBounds.left
                                                    + (viewBounds.width / 4.f)
                                                 , viewBounds.top + 10.f);

    mHUDText.at(HUD::DataType::Enemy).setPosition(viewBounds.left
                                                    + (viewBounds.width / 2.f)
                                                  , viewBounds.top + 10.f);

    mHUDText.at(HUD::DataType::Lives).setPosition(viewBounds.left
                                                    + (viewBounds.width - (viewBounds.width / 4.f))
                                                 , viewBounds.top + 10.f);
}
