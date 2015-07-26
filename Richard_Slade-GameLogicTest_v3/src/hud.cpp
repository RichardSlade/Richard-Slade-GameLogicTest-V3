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

  for (size_t i = 0; i < HUD::DataType::NumDataType; i++)
    mHUDText.push_back(sf::Text("", font));
}

void HUD::draw(sf::RenderTarget& target
  , sf::RenderStates states) const
{
  target.draw(mHUDBackground);

  target.draw(mHUDText.at(HUD::DataType::Level));
  target.draw(mHUDText.at(HUD::DataType::Enemy));
  target.draw(mHUDText.at(HUD::DataType::Lives));
  target.draw(mHUDText.at(HUD::DataType::Score));
}

void HUD::update(int level,
                int enemyNum,
                int playerLives,
                int score)
{
  mHUDText.at(HUD::DataType::Level).setString("Level: " + std::to_string(level));
  mHUDText.at(HUD::DataType::Enemy).setString("Enemies: " + std::to_string(enemyNum));

  mHUDText.at(HUD::DataType::Lives).setString("Lives: " + std::to_string(playerLives));

  mHUDText.at(HUD::DataType::Score).setString("Score: " + std::to_string(score));

  for (sf::Text& txt : mHUDText)
  {
    sf::FloatRect bounds = txt.getLocalBounds();
    txt.setOrigin(bounds.width / 2.f, 0.f);
  }
}

void HUD::setHUDPosition(sf::FloatRect viewBounds)
{
  mHUDBackground.setPosition(viewBounds.left
    , viewBounds.top);

  mHUDText.at(HUD::DataType::Level).setPosition(viewBounds.left + (viewBounds.width / 8.f)
    , viewBounds.top + 10.f);

  mHUDText.at(HUD::DataType::Enemy).setPosition(viewBounds.left + (viewBounds.width / 4.f)
    , viewBounds.top + 10.f);

  mHUDText.at(HUD::DataType::Lives).setPosition(viewBounds.left + (viewBounds.width - (viewBounds.width / 4.f))
    , viewBounds.top + 10.f);

  mHUDText.at(HUD::DataType::Score).setPosition(viewBounds.left + (viewBounds.width - (viewBounds.width / 8.f))
    , viewBounds.top + 10.f);
}
