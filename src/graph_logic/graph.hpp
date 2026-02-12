#pragma once

#include <SFML/Graphics.hpp>
#include "tokenizer.hpp"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <memory>
#include <sstream>
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

enum class TextAnchor {
    CenterTop,
    CenterBottom,
    LeftCenter,
    RightCenter,
};

struct AxisLabel {
    sf::Vector2f position;
    std::string text;
    TextAnchor anchor = TextAnchor::CenterTop;
};

inline double niceTickStep(double rawStep) {
    if (!(rawStep > 0.0) || !std::isfinite(rawStep)) return 1.0;

    const double exponent = std::floor(std::log10(rawStep));
    const double power = std::pow(10.0, exponent);
    const double fraction = rawStep / power;

    double niceFraction = 1.0;
    if (fraction <= 1.0) niceFraction = 1.0;
    else if (fraction <= 2.0) niceFraction = 2.0;
    else if (fraction <= 5.0) niceFraction = 5.0;
    else niceFraction = 10.0;

    return niceFraction * power;
}

inline int decimalsForTickStep(double step) {
    if (!(step > 0.0) || !std::isfinite(step)) return 0;
    const double log10step = std::log10(step);
    if (log10step >= 0.0) return 0;
    return static_cast<int>(std::ceil(-log10step - 1e-12));
}

inline std::string formatTickValue(double value, int decimals) {
    if (!std::isfinite(value)) return "";

    if (std::abs(value) < 1e-12) value = 0.0;

    std::ostringstream oss;
    oss.setf(std::ios::fixed);
    oss << std::setprecision(std::max(0, decimals)) << value;
    std::string s = oss.str();

    if (decimals > 0) {
        while (!s.empty() && s.back() == '0') s.pop_back();
        if (!s.empty() && s.back() == '.') s.pop_back();
    }

    if (s == "-0") s = "0";
    return s;
}

inline void drawAxes(LineBatchRenderer& batch, const Camera& cam, std::vector<AxisLabel>& outLabels) {
    const sf::Vector2f topLeft = cam.screenToGraph(0.0f, 0.0f);
    const sf::Vector2f bottomRight =
        cam.screenToGraph(static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT));

    const float minX = std::min(topLeft.x, bottomRight.x);
    const float maxX = std::max(topLeft.x, bottomRight.x);
    const float minY = std::min(topLeft.y, bottomRight.y);
    const float maxY = std::max(topLeft.y, bottomRight.y);

    const sf::Color axisColor(255, 255, 255, 128); // Semi-transparent
    const sf::Color tickColor(255, 255, 255, 110);

    constexpr float tickLenPx = 6.0f;
    constexpr float labelPadPx = 4.0f;
    constexpr double targetTickSpacingPx = 110.0;

    // X axis (y = 0) if visible
    if (0.0f >= minY && 0.0f <= maxY) {
        const float screenY = cam.graphToScreen(0.0f, 0.0f).y;
        batch.addLine(sf::Vector2f(0.0f, screenY),
                      sf::Vector2f(static_cast<float>(SCREEN_WIDTH), screenY),
                      axisColor);

        const double rawStep = targetTickSpacingPx / static_cast<double>(cam.scaleX);
        const double step = niceTickStep(rawStep);
        const int decimals = decimalsForTickStep(step);

        const long long startIdx = static_cast<long long>(std::floor(static_cast<double>(minX) / step));
        const long long endIdx = static_cast<long long>(std::ceil(static_cast<double>(maxX) / step));

        const bool labelsBelow = screenY + 20.0f < static_cast<float>(SCREEN_HEIGHT);
        const float labelY = labelsBelow ? (screenY + tickLenPx + labelPadPx)
                                         : (screenY - tickLenPx - labelPadPx);
        const TextAnchor anchor = labelsBelow ? TextAnchor::CenterTop : TextAnchor::CenterBottom;

        for (long long i = startIdx; i <= endIdx; ++i) {
            const double x = static_cast<double>(i) * step;
            const float screenX = cam.graphToScreen(static_cast<float>(x), 0.0f).x;

            if (screenX < -20.0f || screenX > static_cast<float>(SCREEN_WIDTH) + 20.0f) continue;

            batch.addLine(sf::Vector2f(screenX, screenY - tickLenPx),
                          sf::Vector2f(screenX, screenY + tickLenPx),
                          tickColor);

            const std::string label = formatTickValue(x, decimals);
            if (label.empty()) continue;
            outLabels.push_back(AxisLabel{sf::Vector2f(screenX, labelY), label, anchor});
        }
    }

    // Y axis (x = 0) if visible
    if (0.0f >= minX && 0.0f <= maxX) {
        const float screenX = cam.graphToScreen(0.0f, 0.0f).x;
        batch.addLine(sf::Vector2f(screenX, 0.0f),
                      sf::Vector2f(screenX, static_cast<float>(SCREEN_HEIGHT)),
                      axisColor);

        const double rawStep = targetTickSpacingPx / static_cast<double>(cam.scaleY);
        const double step = niceTickStep(rawStep);
        const int decimals = decimalsForTickStep(step);

        const long long startIdx = static_cast<long long>(std::floor(static_cast<double>(minY) / step));
        const long long endIdx = static_cast<long long>(std::ceil(static_cast<double>(maxY) / step));

        const bool labelsRight = screenX + 60.0f < static_cast<float>(SCREEN_WIDTH);
        const float labelX = labelsRight ? (screenX + tickLenPx + labelPadPx)
                                         : (screenX - tickLenPx - labelPadPx);
        const TextAnchor anchor = labelsRight ? TextAnchor::LeftCenter : TextAnchor::RightCenter;

        for (long long i = startIdx; i <= endIdx; ++i) {
            const double y = static_cast<double>(i) * step;
            const float screenY = cam.graphToScreen(0.0f, static_cast<float>(y)).y;

            if (screenY < -20.0f || screenY > static_cast<float>(SCREEN_HEIGHT) + 20.0f) continue;

            batch.addLine(sf::Vector2f(screenX - tickLenPx, screenY),
                          sf::Vector2f(screenX + tickLenPx, screenY),
                          tickColor);

            if (std::abs(y) < 1e-12) continue;
            const std::string label = formatTickValue(y, decimals);
            if (label.empty()) continue;
            outLabels.push_back(AxisLabel{sf::Vector2f(labelX, screenY), label, anchor});
        }
    }
}

inline void drawAxes(LineBatchRenderer& batch, const Camera& cam) {
    std::vector<AxisLabel> ignored;
    drawAxes(batch, cam, ignored);
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
