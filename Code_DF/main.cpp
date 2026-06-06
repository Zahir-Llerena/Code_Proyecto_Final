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

// ============================================================================
// 3. CLASE SHIP (Entidad del Jugador)
// ============================================================================
class Ship : public QGraphicsPixmapItem {
public:
    bool moving_right;
    bool moving_left;
    Settings* settings;
    QRectF screen_rect;

    Ship(Settings* settings_ptr, const QRectF& rect_pantalla, const QPixmap& pixmap_textura)
        : moving_right(false), moving_left(false), settings(settings_ptr), screen_rect(rect_pantalla)
    {
        setPixmap(pixmap_textura);
        center_ship();
    }

    void center_ship() {
        // Ecuación geométrica para centrado exacto en eje X
        float x_centro = (screen_rect.width() - boundingRect().width()) / 2.0f;
        float y_inferior = screen_rect.height() - boundingRect().height();
        setPos(x_centro, y_inferior);
    }

    void update_position() {
        if (moving_right && (x() + boundingRect().width() < screen_rect.right())) {
            moveBy(settings->ship_speed, 0);
        }
        if (moving_left && (x() > screen_rect.left())) {
            moveBy(-settings->ship_speed, 0);
        }
    }
};


// ============================================================================
// 4. CLASE BULLET (Cinemática de Proyectiles)
// ============================================================================
class Bullet : public QGraphicsRectItem {
public:
    Settings* settings;

    Bullet(Settings* settings_ptr, Ship* ship) : settings(settings_ptr) {
        setRect(0, 0, settings->bullet_width, settings->bullet_height);
        setBrush(QBrush(settings->bullet_color));
        setPen(Qt::NoPen);

        // Alineación del cañón respecto al centroide de la nave
        float x_inicial = ship->x() + (ship->boundingRect().width() / 2.0f) - (settings->bullet_width / 2.0f);
        float y_inicial = ship->y() - settings->bullet_height;
        setPos(x_inicial, y_inicial);
    }

    void update_position() {
        moveBy(0, -settings->bullet_speed);
    }
};


// ============================================================================
// 5. CLASE ALIEN (Invasores y Detección de Bordes)
// ============================================================================
class Alien : public QGraphicsPixmapItem {
public:
    Settings* settings;
    QRectF screen_rect;

    Alien(Settings* settings_ptr, const QRectF& rect_pantalla, const QPixmap& pixmap_textura)
        : settings(settings_ptr), screen_rect(rect_pantalla)
    {
        setPixmap(pixmap_textura);
    }

    // El sufijo 'const' asegura a Clang que esta función no altera la memoria del objeto
    bool check_edges() const {
        return (x() + boundingRect().width() >= screen_rect.right() || x() <= screen_rect.left());
    }

    void update_position() {
        moveBy(settings->alien_speed * settings->fleet_direction, 0);
    }
};


// ============================================================================
// 6. CLASE SCOREBOARD (Interfaz de Datos Síncrona)
// ============================================================================
class Scoreboard {
private:
    QGraphicsScene* scene;
    GameStats* stats;
    Settings* settings;

    QGraphicsTextItem* score_text;
    QGraphicsTextItem* high_score_text;
    QGraphicsTextItem* level_text;
    QGraphicsTextItem* lives_text;

public:
    Scoreboard(QGraphicsScene* scene_ptr, GameStats* stats_ptr, Settings* settings_ptr)
        : scene(scene_ptr), stats(stats_ptr), settings(settings_ptr)
    {
        QFont font("Courier", 20, QFont::Bold);

        score_text = new QGraphicsTextItem();
        high_score_text = new QGraphicsTextItem();
        level_text = new QGraphicsTextItem();
        lives_text = new QGraphicsTextItem();

        score_text->setFont(font);
        high_score_text->setFont(font);
        level_text->setFont(font);
        lives_text->setFont(font);

        score_text->setDefaultTextColor(QColor(30, 30, 30));
        high_score_text->setDefaultTextColor(QColor(30, 30, 30));
        level_text->setDefaultTextColor(QColor(30, 30, 30));
        lives_text->setDefaultTextColor(QColor(200, 30, 30));

        scene->addItem(score_text);
        scene->addItem(high_score_text);
        scene->addItem(level_text);
        scene->addItem(lives_text);

        score_text->setPos(settings->screen_width - 250, 20);
        high_score_text->setPos(settings->screen_width / 2 - 100, 20);
        level_text->setPos(settings->screen_width - 250, 50);
        lives_text->setPos(30, 20);

        update_scoreboard();
    }

    void update_scoreboard() {
        score_text->setPlainText("SCORE: " + QString::number(stats->score));
        high_score_text->setPlainText("HI-SCORE: " + QString::number(stats->high_score));
        level_text->setPlainText("LEVEL: " + QString::number(stats->level));
        lives_text->setPlainText("SHIPS: " + QString::number(stats->ships_left));
    }
};


