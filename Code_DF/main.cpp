// ============================================================================
// LIBRERÍAS ESTÁNDAR DE C++ (STL)
// ============================================================================
#include <iostream>
#include <vector>
#include <memory>
#include <cmath>

// ============================================================================
// MÓDULOS DE QT6 CORE Y GUI
// ============================================================================
#include <QApplication>
#include <QTimer>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QFileInfo>
#include <QDir>
#include <QCoreApplication>

// ============================================================================
// MÓDULOS DEL GRAPHICS VIEW FRAMEWORK (Motor de Renderizado Vectorial)
// ============================================================================
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsProxyWidget>
#include <QFont>

// ============================================================================
// 1. CLASE SETTINGS (Parámetros Dinámicos y Físicos)
// ============================================================================
class Settings {
public:
    int screen_width;
    int screen_height;
    QColor bg_color;
    int ship_limit;
    int bullet_width;
    int bullet_height;
    QColor bullet_color;
    int bullets_allowed;

    float speedup_scale;
    float score_scale;

    float ship_speed;
    float bullet_speed;
    float alien_speed;
    float fleet_drop_speed;
    int fleet_direction;
    int alien_points;

    // OPTIMIZACIÓN: Lista de inicialización para prevenir doble asignación en memoria
    Settings()
        : screen_width(1200), screen_height(800), bg_color(230, 230, 230),
        ship_limit(3), bullet_width(3), bullet_height(15),
        bullet_color(60, 60, 60), bullets_allowed(17),
        speedup_scale(1.25f), score_scale(1.5f)
    {
        initialize_dynamic_settings();
    }

    virtual ~Settings() {}

    void initialize_dynamic_settings() {
        ship_speed = 1.5f;
        bullet_speed = 4.0f;
        alien_speed = 1.0f;
        fleet_drop_speed = 12.0f;
        fleet_direction = 1;
        alien_points = 50;
    }

    void increase_speed() {
        ship_speed *= speedup_scale;
        bullet_speed *= speedup_scale;
        alien_speed *= speedup_scale;
        fleet_drop_speed *= speedup_scale;
        alien_points = static_cast<int>(alien_points * score_scale);
    }
};


// ============================================================================
// 2. CLASE GAMESTATS (Máquina de Estados Finita)
// ============================================================================
class GameStats {
public:
    Settings* settings;
    int ships_left;
    int score;
    int level;
    int high_score;
    bool game_active;

    GameStats(Settings* settings_ptr) : settings(settings_ptr), high_score(0), game_active(false) {
        reset_stats();
    }

    void reset_stats() {
        ships_left = settings->ship_limit;
        score = 0;
        level = 1;
    }
};











