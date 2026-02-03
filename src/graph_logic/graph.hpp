#include <SFML/Graphics.hpp>
#include "tokenizer.hpp"
#include <sstream>
#include <cmath>  // for std::hypot, std::atan2
const int SCREEN_WIDTH=1920;
const int SCREEN_HEIGHT=1080;
double originX=SCREEN_WIDTH/2.0;
double originY=SCREEN_HEIGHT/2.0;

enum colorpick{
	white,
	black,
	blue,
	red,
    yellow,
    green,
	colorpick_unknown
};

// stringToEnum: convert input string to customer choice enum (safe fallback to unknown)
colorpick stringToEnum(const std::string& input) {
	static std::unordered_map<std::string, colorpick> colorpickMap = {
		{"white", colorpick::white},
		{"black", colorpick::black},
		{"blue", colorpick::blue},
		{"red", colorpick::red},
		{"yellow", colorpick::yellow},
		{"green", colorpick::green},
	};

	auto it = colorpickMap.find(input);
	return (it != colorpickMap.end()) ? it->second : colorpick::colorpick_unknown;
}
sf:: Color select_color(string color){
    colorpick color_pick=stringToEnum(color);
    switch (color_pick)
    {
    case white:
        return sf::Color::White;
        break;
    case black:
        return sf::Color::Black;
        break;
    case blue:
        return sf::Color::Blue;
        break;
    case red:
        return sf::Color::Red;
        break;
    case yellow:
        return sf::Color::Yellow;
        break;
    case green:
        return sf::Color::Green;
        break;
    default:
        return sf::Color::White;
        break;
    }
}

void DrawLine(float x1, float y1, float x2, float y2, string color, sf::RenderWindow& window)
{
    // Make an array of the two end points (an array is like a list)
    sf::Vertex points[2];

    points[0].position = sf::Vector2f(x1, y1);
    points[1].position = sf::Vector2f(x2, y2);

    points[0].color = select_color(color);
    points[1].color = select_color(color);

    // Draw the line made by the two end points
    window.draw(points, 2, sf::PrimitiveType::Lines);
}

void Drawminiline(float x1, float y1, float x2, float y2, string color, sf::RenderWindow& window,float scalex,float scaley){
    double screenX1=originX+(x1*scalex);
    double screenY1=originY-(y1*scaley);
    double screenX2=originX+(x2*scalex);
    double screenY2=originY-(y2*scaley);
    if(!(std::isfinite(screenY1)) || !(std::isfinite(screenY2)))return;
    if(abs(screenY1-screenY2)>SCREEN_HEIGHT)return;
    DrawLine(screenX1,screenY1,screenX2,screenY2,color,window);
}