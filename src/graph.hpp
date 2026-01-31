#include <SFML/Graphics.hpp>
#include "tokenizer.hpp"
#include <sstream>
#include <cmath>  // for std::hypot, std::atan2
const int SCREEN_WIDTH=2560;
const int SCREEN_HEIGHT=1440;
double originX=SCREEN_WIDTH/2.0;
double originY=SCREEN_HEIGHT/2.0;

void DrawLine(float x1, float y1, float x2, float y2, sf::Color color, sf::RenderWindow& window)
{
    // Make an array of the two end points (an array is like a list)
    sf::Vertex points[2];

    points[0].position = sf::Vector2f(x1, y1);
    points[1].position = sf::Vector2f(x2, y2);

    points[0].color = color;
    points[1].color = color;

    // Draw the line made by the two end points
    window.draw(points, 2, sf::PrimitiveType::Lines);
}

void Drawminiline(float x1, float y1, float x2, float y2, sf::Color color, sf::RenderWindow& window,float scalex,float scaley){
    double screenX1=originX+(x1*scalex);
    double screenY1=originY-(y1*scaley);
    double screenX2=originX+(x2*scalex);
    double screenY2=originY-(y2*scaley);

    DrawLine(screenX1,screenY1,screenX2,screenY2,color,window);
}