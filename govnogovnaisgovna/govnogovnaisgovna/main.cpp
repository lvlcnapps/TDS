#include <SFML/Graphics.hpp>
#include <iostream>
#include <math.h>
#include <ctime>

#define pi 3.1415

using namespace std;

float playerSpeed = 4, bulletSpeed = 12;

int rotation(int mouse_x, int mouse_y, int pl_x, int pl_y) {
    return atan2((mouse_y - pl_y), (mouse_x - pl_x)) / pi * 180;
}

struct bulletO{
    float x, y, angle;
    int created;
};

int main() {
    float pl_x = 0, pl_y = 0;
    clock_t flagTime = clock();
    int can_shoot = 1, timer = 0, last_time = -1, cooldownShoot = 200;
    vector <bulletO> bullets;
    sf::RenderWindow window(sf::VideoMode(640, 480), "SFML works!");
    sf::Texture texture;
    if (!texture.loadFromFile("player.png")) {
        cout << "5";
    }
    sf::Texture bulletT;
    if (!bulletT.loadFromFile("bullet.png")) {
        cout << "5";
    }
    bulletT.setSmooth(true);

    sf::Sprite sprite;
    sprite.setTexture(texture);
    sprite.setOrigin(sf::Vector2f(13.f, 13.f));

    /*sf::Sprite bulletS;
    bulletS.setTexture(bulletT);
    bulletS.setOrigin(sf::Vector2f(30.f, 12.f));*/

    while (window.isOpen()) {
        clock_t nowTime = clock();
        cout << nowTime << endl;
        if (nowTime - flagTime > 20) {
            flagTime = nowTime;
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
            }

            sf::Vector2i localPosition = sf::Mouse::getPosition(window);
            sprite.setRotation(rotation(localPosition.x, localPosition.y, pl_x, pl_y));
            //cout << rotation(localPosition.x, localPosition.y, pl_x, pl_y) << " " << pl_x << " " << pl_y << endl;

            if ((can_shoot == 1) && (sf::Mouse::isButtonPressed(sf::Mouse::Left))) {
                bulletO bul;
                bul.x = pl_x;
                bul.y = pl_y;
                bul.angle = rotation(localPosition.x, localPosition.y, pl_x, pl_y);
                bul.created = 1;
                bullets.push_back(bul);
                can_shoot = 0;
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
            window.draw(sprite);
            for (bulletO& i : bullets) {
                sf::Sprite bulletSP;
                bulletSP.setTexture(bulletT);
                bulletSP.setOrigin(sf::Vector2f(30.f, 12.f));
                //cout << i.created << endl;
                /*if (i.created == 1) {
                    bulletSP.setPosition(i.x, i.y);
                    i.created = 0;
                    //cout << "7";
                } else {
                    bulletSP.move(cos(i.angle) * bulletSpeed, sin(i.angle) * bulletSpeed);
                }*/
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