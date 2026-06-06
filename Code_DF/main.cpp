// ============================================================================
// Universidad de Antioquia
// Facultad de Ingenieria - Departamento de Telecomunicaciones -
// @author:
//         Arlington Zahir Llerena CC: 72 277 711
// email: zahir.llerena@udea.edu.co
// Informatica II - Semestre - 3
// Desafio final: - Juego - Invacion Alien
// 6 de junio de 2026
// ============================================================================


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
// 1. CLASE SETTINGS (Parámetros Dinámicos y Físicos - Estructura Clásica C++)
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

    // Constructor clásico con lista de inicialización para optimización en RAM
    Settings()
        : screen_width(1200), screen_height(800), bg_color(230, 230, 230),
        ship_limit(3), bullet_width(3), bullet_height(15),
        bullet_color(60, 60, 60), bullets_allowed(17),
        speedup_scale(1.25f), score_scale(1.5f)
    {
        inicializar_ajustes_dinamicos();
    }

    // REDISEÑO BÁSICO: Destructor plano no virtual.
    // Al ser una clase final sin herencia, no requiere la vtable polimórfica.
    ~Settings() {}

    void inicializar_ajustes_dinamicos() {
        ship_speed = 1.5f;
        bullet_speed = 4.0f;
        alien_speed = 1.0f;
        fleet_drop_speed = 12.0f;
        fleet_direction = 1;
        alien_points = 50;
    }

    void aumentar_velocidad() {
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
        reiniciar_estadisticas();
    }

    void reiniciar_estadisticas() {
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
        centrar_nave();
    }

    void centrar_nave() {
        float x_centro = (screen_rect.width() - boundingRect().width()) / 2.0f;
        float y_inferior = screen_rect.height() - boundingRect().height();
        setPos(x_centro, y_inferior);
    }

    void actualizar_posicion() {
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

        float x_inicial = ship->x() + (ship->boundingRect().width() / 2.0f) - (settings->bullet_width / 2.0f);
        float y_inicial = ship->y() - settings->bullet_height;
        setPos(x_inicial, y_inicial);
    }

    void actualizar_posicion() {
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

    bool tocar_limites() const {
        return (x() + boundingRect().width() >= screen_rect.right() || x() <= screen_rect.left());
    }

    void actualizar_posicion() {
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

        actualizar_marcador();
    }

    void actualizar_marcador() {
        score_text->setPlainText("SCORE: " + QString::number(stats->score));
        high_score_text->setPlainText("HI-SCORE: " + QString::number(stats->high_score));
        level_text->setPlainText("LEVEL: " + QString::number(stats->level));
        lives_text->setPlainText("SHIPS: " + QString::number(stats->ships_left));
    }
};

// ============================================================================
// CLASE PLAYBUTTON (Vector Gráfico Interactivo)
// ============================================================================
class PlayButton : public QGraphicsRectItem {
private:
    QGraphicsTextItem* text_item;

public:
    PlayButton(float width, float height, float screen_w, float screen_h) {
        setRect(0, 0, width, height);
        setBrush(QBrush(QColor(0, 135, 68)));
        setPen(Qt::NoPen);
        setPos((screen_w - width) / 2.0f, (screen_h - height) / 2.0f);

        text_item = new QGraphicsTextItem("HAGALE PARCERO - PLAY -", this);
        QFont font("Helvetica", 24, QFont::Bold);
        text_item->setFont(font);
        text_item->setDefaultTextColor(Qt::white);

        float text_w = text_item->boundingRect().width();
        float text_h = text_item->boundingRect().height();
        text_item->setPos((width - text_w) / 2.0f, (height - text_h) / 2.0f);
    }
};
// ============================================================================
// 7. MOTOR DE JUEGO PRINCIPAL (Orquestador Estricto)
// ============================================================================
class AlienInvasion : public QGraphicsView {
    Q_OBJECT

private:
    Settings* settings;
    GameStats* stats;
    QGraphicsScene* scene;
    Scoreboard* sb;
    Ship* ship;
    PlayButton* play_button;
    std::vector<Bullet*> bullets;
    std::vector<Alien*> fleet;
    QTimer* game_timer;

    QPixmap pixmap_ship;
    QPixmap pixmap_alien;

public:
    AlienInvasion()
        : QGraphicsView(),
        settings(new Settings()),
        stats(new GameStats(settings)),
        scene(new QGraphicsScene(this)),
        sb(nullptr),
        ship(nullptr),
        play_button(nullptr),
        game_timer(new QTimer(this))
    {
        scene->setSceneRect(0, 0, settings->screen_width, settings->screen_height);
        scene->setBackgroundBrush(settings->bg_color);
        setScene(scene);

        setFixedSize(settings->screen_width, settings->screen_height);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setWindowTitle("Motor C++: Alien Invasion - Informatica II - UdeA.");

        QFileInfo source_info(__FILE__);
        QDir project_dir = source_info.absoluteDir();

        QString ruta_ship = project_dir.absoluteFilePath("images/ship.bmp");
        QString ruta_alien = project_dir.absoluteFilePath("images/alien.bmp");

        pixmap_ship.load(ruta_ship);
        pixmap_alien.load(ruta_alien);

        ship = new Ship(settings, scene->sceneRect(), pixmap_ship);
        scene->addItem(ship);

        sb = new Scoreboard(scene, stats, settings);

        bullets.reserve(settings->bullets_allowed);

        armar_la_flota();

        play_button = new PlayButton(480, 60, settings->screen_width, settings->screen_height);
        scene->addItem(play_button);

        setCursor(Qt::ArrowCursor);

        connect(game_timer, &QTimer::timeout, this, &AlienInvasion::ciclo_principal);
        game_timer->start(16);
    }

    ~AlienInvasion() {
        for (Bullet* b : bullets) delete b;
        bullets.clear();
        for (Alien* a : fleet) delete a;
        fleet.clear();
        delete stats;
        delete sb;
        if (settings) delete settings; // Llama de manera segura al nuevo destructor clásico
    }

protected:
    void mousePressEvent(QMouseEvent *event) override {
        if (!stats->game_active && event->button() == Qt::LeftButton) {
            QPointF mouse_pos = mapToScene(event->pos());

            if (play_button && play_button->contains(play_button->mapFromScene(mouse_pos))) {
                stats->game_active = true;
                stats->reiniciar_estadisticas();
                settings->inicializar_ajustes_dinamicos();
                sb->actualizar_marcador();

                setCursor(Qt::BlankCursor);
                scene->removeItem(play_button);

                for (Bullet* b : bullets) { scene->removeItem(b); delete b; }
                bullets.clear();
                for (Alien* a : fleet) { scene->removeItem(a); delete a; }
                fleet.clear();

                armar_la_flota();
                ship->centrar_nave();
            }
        }
        QGraphicsView::mousePressEvent(event);
    }

    void keyPressEvent(QKeyEvent *event) override {
        if (!stats->game_active || !ship) return;

        if (event->key() == Qt::Key_Right) {
            ship->moving_right = true;
        } else if (event->key() == Qt::Key_Left) {
            ship->moving_left = true;
        } else if (event->key() == Qt::Key_Space) {
            disparar_proyectil();
        } else if (event->key() == Qt::Key_Q) {
            QApplication::quit();
        }
    }

    void keyReleaseEvent(QKeyEvent *event) override {
        if (!ship) return;
        if (event->key() == Qt::Key_Right) {
            ship->moving_right = false;
        } else if (event->key() == Qt::Key_Left) {
            ship->moving_left = false;
        }
    }

private:
    void disparar_proyectil() {
        if (static_cast<int>(bullets.size()) < settings->bullets_allowed) {
            Bullet* new_bullet = new Bullet(settings, ship);
            scene->addItem(new_bullet);
            bullets.push_back(new_bullet);
        }
    }

    void actualizar_proyectiles() {
        for (auto it = bullets.begin(); it != bullets.end(); ) {
            (*it)->actualizar_posicion();
            if ((*it)->y() + (*it)->rect().height() <= 0) {
                scene->removeItem(*it);
                delete *it;
                it = bullets.erase(it);
            } else {
                ++it;
            }
        }
        revisar_impactos_certeros();
    }

    void revisar_impactos_certeros() {
        for (auto b_it = bullets.begin(); b_it != bullets.end(); ) {
            Bullet* bala_actual = *b_it;
            bool bala_impacto = false;

            QList<QGraphicsItem*> items_chocados = scene->collidingItems(bala_actual);

            for (QGraphicsItem* item : items_chocados) {
                Alien* alien_detectado = dynamic_cast<Alien*>(item);
                if (alien_detectado) {
                    for (auto a_it = fleet.begin(); a_it != fleet.end(); ++a_it) {
                        if (*a_it == alien_detectado) {
                            scene->removeItem(alien_detectado);
                            delete alien_detectado;
                            fleet.erase(a_it);
                            break;
                        }
                    }

                    stats->score += settings->alien_points;
                    if (stats->score > stats->high_score) {
                        stats->high_score = stats->score;
                    }
                    sb->actualizar_marcador();

                    scene->removeItem(bala_actual);
                    delete bala_actual;
                    b_it = bullets.erase(b_it);
                    bala_impacto = true;
                    break;
                }
            }

            if (!bala_impacto) {
                ++b_it;
            }
        }

        if (fleet.empty()) {
            for (Bullet* b : bullets) {
                scene->removeItem(b);
                delete b;
            }
            bullets.clear();

            stats->level += 1;
            sb->actualizar_marcador();

            settings->aumentar_velocidad();
            armar_la_flota();
        }
    }

    void armar_la_flota() {
        float alien_width = pixmap_alien.width();
        float alien_height = pixmap_alien.height();

        float available_space_x = settings->screen_width - (2.0f * alien_width);
        int number_aliens_x = static_cast<int>(available_space_x / (2.0f * alien_width));

        float ship_height = ship->boundingRect().height();
        float available_space_y = settings->screen_height - (3.0f * alien_height) - ship_height;
        int number_rows = static_cast<int>(available_space_y / (2.0f * alien_height));

        fleet.reserve(number_aliens_x * number_rows);

        for (int row_number = 0; row_number < number_rows; ++row_number) {
            for (int alien_number = 0; alien_number < number_aliens_x; ++alien_number) {
                ubicar_un_alien(alien_number, row_number, alien_width, alien_height);
            }
        }
    }

    void ubicar_un_alien(int alien_number, int row_number, float alien_width, float alien_height) {
        Alien* alien = new Alien(settings, scene->sceneRect(), pixmap_alien);
        float x_pos = alien_width + 2.0f * alien_width * alien_number;
        float y_pos = alien_height + 2.0f * alien_height * row_number;

        alien->setPos(x_pos, y_pos);
        scene->addItem(alien);
        fleet.push_back(alien);
    }

    void actualizar_la_flota() {
        revisar_limites_pantalla();
        for (Alien* alien : fleet) {
            alien->actualizar_posicion();
        }
        revisar_invasion_o_choques();
    }

    void revisar_limites_pantalla() {
        for (Alien* alien : fleet) {
            if (alien->tocar_limites()) {
                pegar_la_vuelta_y_descender();
                break;
            }
        }
    }

    void pegar_la_vuelta_y_descender() {
        for (Alien* alien : fleet) {
            alien->moveBy(0, settings->fleet_drop_speed);
        }
        settings->fleet_direction *= -1;
    }

    void revisar_invasion_o_choques() {
        bool invasion_ocurrida = false;

        for (Alien* alien : fleet) {
            if (alien->collidesWithItem(ship)) {
                invasion_ocurrida = true;
                break;
            }
            if (alien->y() + alien->boundingRect().height() >= settings->screen_height) {
                invasion_ocurrida = true;
                break;
            }
        }

        if (invasion_ocurrida) {
            sufrir_una_baja();
        }
    }

    void sufrir_una_baja() {
        if (stats->ships_left > 1) {
            stats->ships_left -= 1;
            sb->actualizar_marcador();

            for (Bullet* b : bullets) { scene->removeItem(b); delete b; }
            bullets.clear();
            for (Alien* a : fleet) { scene->removeItem(a); delete a; }
            fleet.clear();

            armar_la_flota();
            ship->centrar_nave();

            QTimer::singleShot(500, [](){});
        } else {
            stats->ships_left = 0;
            sb->actualizar_marcador();
            stats->game_active = false;

            if (play_button) {
                scene->addItem(play_button);
            }
            setCursor(Qt::ArrowCursor);
            setWindowTitle("Alien Invasion - ¡GAME OVER! Presiona PLAY para reiniciar");
        }
    }

private slots:
    void ciclo_principal() {
        if (!stats->game_active) return;

        if (ship) {
            ship->actualizar_posicion();
        }
        actualizar_proyectiles();
        actualizar_la_flota();
    }
};

// ============================================================================
// 8. PUNTO DE ENTRADA PRINCIPAL DE LA APLICACIÓN
// ============================================================================
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    AlienInvasion game;
    game.show();

    return app.exec();
}

#include "main.moc"


// ============================================================================
// Muchas gracias senor profesor por la oportunidad de realizar este trabajo.
// Desafortunadamente no pude darles respuesta a todos los item que solicito
// en la guia. Quede frustrado por el tiempo inivertido para reproducir una
// pista en formato .mp3 durante la ejecucion del juego y al final no puede
// adaptar el sonido al juego. No obstante, creo que le di respuestas a los
// item  mas importantes en la iniciativa. Agradecemos el espacio brindado.
// ============================================================================





