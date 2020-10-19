#include <SFML/Graphics.hpp>
#include <iostream>
#include <math.h>
#include <ctime>
#include <fstream>

#define pi 3.1415

using namespace std;

/*int map1[15][20] {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};*/
int map1[15][20];

float playerSpeed = 2, bulletSpeed = 6;
int hp = 100, ammo = 30;

int rotation(int mouse_x, int mouse_y, int pl_x, int pl_y) {
    return atan2((mouse_y - pl_y), (mouse_x - pl_x)) / pi * 180;
}

struct bulletO{
    float x, y, angle;
    int created;
};

int main() {
    ifstream fin("map.m");
    for (int o = 0; o < 15; o++) {
        for (int e = 0; e < 20; e++) {
            fin >> map1[o][e];
            if (map1[o][e] == 1) {
                cout << "Block\n";
            }
        }
    }
    fin.close();
    float pl_x = 0, pl_y = 0;
    clock_t flagTime = clock();
    int can_shoot = 1, timer = 0, last_time = -1, cooldownShoot = 50;
    vector <bulletO> bullets;
    sf::RenderWindow window(sf::VideoMode(640, 480), "TDS");
    sf::Texture texture;
    if (!texture.loadFromFile("player.png")) {
        cout << "5";
    }
    sf::Texture enemyT;
    if (!enemyT.loadFromFile("enemy.png")) {
        cout << "5";
    }
    sf::Texture bulletT;
    if (!bulletT.loadFromFile("bullet.png")) {
        cout << "5";
    }
    sf::Texture backGT;
    if (!backGT.loadFromFile("background1.png")) {
        cout << "5";
    }
    sf::Texture blockT;
    if (!blockT.loadFromFile("wall.png")) {
        cout << "5";
    }

    for (int o = 0; o < 15; o++) {
        for (int e = 0; e < 20; e++) {
            switch (map1[o][e]) {
                case 1:
                    sf::Sprite blockS;
                    blockS.setTexture(blockT);
                    blockS.setPosition(e * 32, o * 32);
                    //blockS.setPosition(0, 0);
                    window.draw(blockS);
            }
        }
    }

    bulletT.setSmooth(true);

    sf::Sprite sprite;
    sprite.setTexture(texture);
    sprite.setOrigin(sf::Vector2f(13.f, 13.f));

    sf::Sprite backS;
    backS.setTexture(backGT);

    sf::View view(sf::FloatRect(0.f, 0.f, 640.f, 480.f));
    window.setView(view);

    while (window.isOpen()) {
        clock_t nowTime = clock();
        //cout << nowTime << endl;
        if (nowTime - flagTime > 10) {
            int fps = 1000 / (nowTime - flagTime);
            //cout << fps << endl;
            flagTime = nowTime;
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
                if (event.type == sf::Event::Resized)
                {
                    sf::FloatRect visibleArea(0.f, 0.f, event.size.width, event.size.height);
                    window.setView(sf::View(visibleArea));
                }
            }

            sf::Vector2i localPosition = sf::Mouse::getPosition(window);
            sprite.setRotation(rotation(localPosition.x, localPosition.y, pl_x, pl_y));
            //cout << rotation(localPosition.x, localPosition.y, pl_x, pl_y) << " " << pl_x << " " << pl_y << endl;

            if ((ammo > 0) && (can_shoot == 1) && (sf::Mouse::isButtonPressed(sf::Mouse::Left))) {
                bulletO bul;
                bul.x = pl_x;
                bul.y = pl_y;
                bul.angle = rotation(localPosition.x, localPosition.y, pl_x, pl_y);
                bul.created = 1;
                bullets.push_back(bul);
                can_shoot = 0;
                ammo--;
                cout << "6";
                last_time = timer;
            }
            //cout << sin(rotation(localPosition.x, localPosition.y, pl_x, pl_y)) << endl;
            //cout << sin(180 / 180 * pi) << endl;
            timer++;
            if ((last_time != -1) && (timer - last_time == cooldownShoot)) {
                can_shoot = 1;
                last_time = -1;
                timer = 0;
            }
            
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) {
                view.move(100.f, 0.f);
                cout << "Z\n";
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::X)) {
                view.rotate(5.f);
                cout << "X\n";
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
                sprite.move(sf::Vector2f(-playerSpeed, 0.f));
                pl_x -= playerSpeed;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
                sprite.move(sf::Vector2f(playerSpeed, 0.f));
                pl_x += playerSpeed;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
                sprite.move(sf::Vector2f(0.f, -playerSpeed));
                pl_y -= playerSpeed;
                /*if (sprite.getGlobalBounds().intersects(blockS.getGlobalBounds())) {
                    sprite.move(sf::Vector2f(0.f, playerSpeed));
                    pl_y += playerSpeed;
                    cout << "Collision\n";
                }*/
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
                sprite.move(sf::Vector2f(0.f, playerSpeed));
                pl_y += playerSpeed;
            }
            int y = 0;
            for (bulletO i : bullets) {
                if ((i.x < 0) || (i.x > 640) || (i.y < 0) || (i.y > 480)) {
                    bullets.erase(bullets.begin() + y);
                    cout << "8" << endl;
                }
                y++;
            }

            window.clear();
            window.draw(backS);
            for (int o = 0; o < 15; o++) {
                for (int e = 0; e < 20; e++) {
                    if (map1[o][e] == 1) {
                        sf::Sprite blockS;
                        blockS.setTexture(blockT);
                        blockS.setPosition(e * 32, o * 32);
                        window.draw(blockS);
                    }
                    if (map1[o][e] == 2) {
                        sf::Sprite enemyS;
                        enemyS.setTexture(enemyT);
                        enemyS.setOrigin(sf::Vector2f(19.f, 13.f));
                        enemyS.setPosition(e * 32, o * 32);
                        enemyS.setRotation(rotation(e * 32, o * 32, pl_x, pl_y) + 180);
                        window.draw(enemyS);
                    }
                }
            }
            window.draw(sprite);
            for (bulletO& i : bullets) {
                sf::Sprite bulletSP;
                bulletSP.setTexture(bulletT);
                bulletSP.setOrigin(sf::Vector2f(10.f, 12.f));
                bulletSP.setPosition(i.x, i.y);
                i.x += cos(i.angle / 180 * pi) * bulletSpeed;
                i.y += sin(i.angle / 180 * pi) * bulletSpeed;
                bulletSP.setRotation(i.angle);
                window.draw(bulletSP);
            }
            window.display();
        }
    }
    return 0;
}