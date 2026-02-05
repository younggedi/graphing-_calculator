#pragma once

#include <SFML/Graphics.hpp>
#include "tokenizer.hpp"

#include <algorithm>
#include <cmath>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

inline constexpr int SCREEN_WIDTH = 1920;
inline constexpr int SCREEN_HEIGHT = 1080;
inline float originX = SCREEN_WIDTH / 2.0f;
inline float originY = SCREEN_HEIGHT / 2.0f;

// Camera/View system for better zoom and pan
struct Camera {
    float offsetX = 0.0f;
    float offsetY = 0.0f;
    float scaleX = 50.0f;
    float scaleY = 20.0f;

    // Convert graph coordinates to screen coordinates
    sf::Vector2f graphToScreen(float graphX, float graphY) const {
        return sf::Vector2f(
            originX + (graphX + offsetX) * scaleX,
            originY - (graphY + offsetY) * scaleY
        );
    }

    // Convert screen coordinates to graph coordinates
    sf::Vector2f screenToGraph(float screenX, float screenY) const {
        return sf::Vector2f(
            (screenX - originX) / scaleX - offsetX,
            -(screenY - originY) / scaleY - offsetY
        );
    }
};

// Batch rendering approach
struct BatchLine {
    sf::Vertex vertices[2];
};

class LineBatchRenderer {
private:
    std::vector<sf::Vertex> m_vertices;
    sf::PrimitiveType m_primitiveType;
    
public:
    LineBatchRenderer() : m_primitiveType(sf::PrimitiveType::Lines) {
        m_vertices.reserve(10000); // Pre-allocate memory
    }
    
    void clear() {
        m_vertices.clear();
    }
    
    void addLine(const sf::Vector2f& p1, const sf::Vector2f& p2, const sf::Color& color) {
        m_vertices.push_back(sf::Vertex{p1, color});
        m_vertices.push_back(sf::Vertex{p2, color});
    }
    
    void render(sf::RenderWindow& window) {
        if (!m_vertices.empty()) {
            window.draw(m_vertices.data(), m_vertices.size(), m_primitiveType);
        }
    }
    
    size_t size() const { return m_vertices.size(); }
};

// Pre-convert colors to avoid string parsing every frame
class ColorManager {
private:
    std::unordered_map<std::string, sf::Color> m_colorCache;
    
public:
    ColorManager() {
        // Initialize with common colors
        m_colorCache["white"] = sf::Color::White;
        m_colorCache["black"] = sf::Color::Black;
        m_colorCache["blue"] = sf::Color::Blue;
        m_colorCache["red"] = sf::Color::Red;
        m_colorCache["yellow"] = sf::Color::Yellow;
        m_colorCache["green"] = sf::Color::Green;
    }
    
    const sf::Color& getColor(const std::string& colorName) {
        auto it = m_colorCache.find(colorName);
        if (it != m_colorCache.end()) {
            return it->second;
        }
        return m_colorCache["white"]; // Default fallback
    }
};

inline void drawAxes(LineBatchRenderer& batch, const Camera& cam) {
    const sf::Vector2f topLeft = cam.screenToGraph(0.0f, 0.0f);
    const sf::Vector2f bottomRight =
        cam.screenToGraph(static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT));

    const float minX = std::min(topLeft.x, bottomRight.x);
    const float maxX = std::max(topLeft.x, bottomRight.x);
    const float minY = std::min(topLeft.y, bottomRight.y);
    const float maxY = std::max(topLeft.y, bottomRight.y);

    const sf::Color axisColor(255, 255, 255, 128); // Semi-transparent

    // X axis (y = 0) if visible
    if (0.0f >= minY && 0.0f <= maxY) {
        const float screenY = cam.graphToScreen(0.0f, 0.0f).y;
        batch.addLine(sf::Vector2f(0.0f, screenY),
                      sf::Vector2f(static_cast<float>(SCREEN_WIDTH), screenY),
                      axisColor);
    }

    // Y axis (x = 0) if visible
    if (0.0f >= minX && 0.0f <= maxX) {
        const float screenX = cam.graphToScreen(0.0f, 0.0f).x;
        batch.addLine(sf::Vector2f(screenX, 0.0f),
                      sf::Vector2f(screenX, static_cast<float>(SCREEN_HEIGHT)),
                      axisColor);
    }
}

inline void drawGraphLines(const std::vector<std::vector<double>>& xvalues,
                           const std::vector<std::vector<double>>& yvalues,
                           const std::vector<std::string>& color_list,
                           LineBatchRenderer& batch,
                           ColorManager& colorManager,
                           const Camera& cam) {
    for (size_t no = 0; no < xvalues.size(); ++no) {
        const auto& xvals = xvalues[no];
        const auto& yvals = yvalues[no];

        const sf::Color& color = (no < color_list.size())
            ? colorManager.getColor(color_list[no])
            : sf::Color::White;
        
        if (xvals.size() <= 1 || xvals.size() != yvals.size()) continue;
        
        for (size_t i = 0; i < xvals.size() - 1; ++i) {
            const double y1 = yvals[i];
            const double y2 = yvals[i + 1];
            if (!std::isfinite(y1) || !std::isfinite(y2)) continue;

            const sf::Vector2f p1 = cam.graphToScreen(static_cast<float>(xvals[i]), static_cast<float>(y1));
            const sf::Vector2f p2 = cam.graphToScreen(static_cast<float>(xvals[i + 1]), static_cast<float>(y2));

            if (std::abs(p1.y - p2.y) > static_cast<float>(SCREEN_HEIGHT) * 2.0f) continue;
            
            batch.addLine(p1,
                         p2,
                         color);
        }
    }
}

// Main game loop
