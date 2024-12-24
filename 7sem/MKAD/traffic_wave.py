import pygame
import scipy.stats as stats


def TimeGenerator(mu, size=100):
    for xi in stats.poisson.rvs(mu * 1000, size=size):
        yield xi / 1000


def SpeedGenerator(mu, var, size=100):
    for xi in stats.norm.rvs(mu, var, size=size):
        yield xi


class Car(pygame.sprite.Sprite):

    def bind(self):
        self.app.sprites.add(self)

    def __init__(self, app, w=300, h=200):
        pygame.sprite.Sprite.__init__(self)
        self.speed = None
        self.acceleration = None
        self.app = app
        self.image = pygame.image.load('car.png')
        self.image = pygame.transform.scale(self.image, (w, h))
        self.rect = self.image.get_rect()
        self.image.blit(self.image, self.rect)
        self.rect.right = 0
        self.rect.centery = app.height // 2
        self.bind()

    def update(self):
        a = self.acceleration
        t = self.app.delta_time

        self.speed = min(max(self.speed + a * t / 2, 1), 2.4)
        self.rect.move_ip(self.app.FPS_lock * self.speed * t, 0)

        if self.rect.left > app.width:
            self.app.car_list.pop(-1)
            self.kill()


class App:

    def __init__(self, width=300, height=200, FPS_lock=80):
        self.car_list = None
        pygame.init()
        self.screen = pygame.display.set_mode((width, height))
        self.width, self.height = width, height
        self.FPS_lock = FPS_lock
        pygame.display.set_caption('Будни МКАДА')
        self.clock = pygame.time.Clock()
        self.sprites = pygame.sprite.Group()

    def run(self):
        timer = TimeGenerator(5)
        speeds = SpeedGenerator(2.5, 0.05)
        acceleration = 1.2
        optimal_dist = 70
        time_since_last = pygame.time.get_ticks() / 1000
        time_to_wait = 0
        self.car_list = []
        running, end = True, False

        while running:
            self.delta_time = self.clock.tick(self.FPS_lock) / 1000
            # # generate a new one?
            time = pygame.time.get_ticks() / 1000
            if not end and time - time_since_last > time_to_wait:
                car = Car(self)
                car.speed = next(speeds)
                car.acceleration = 0
                self.car_list.insert(0, car)

                try:
                    time_to_wait = next(timer)
                except:
                    end = True

                time_since_last = pygame.time.get_ticks() / 1000

            for i, car in enumerate(self.car_list[:-1]):
                next_car = self.car_list[i + 1]
                dist = next_car.rect.left - car.rect.right

                if dist < optimal_dist + optimal_dist // 3:
                    car.acceleration = -acceleration
                elif dist > optimal_dist:
                    car.acceleration = acceleration
                else:
                    car.acceleration = 0

            if self.car_list:
                self.car_list[-1].acceleration = acceleration

            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    running = False

                keys = pygame.key.get_pressed()

                if keys[pygame.K_SPACE]:
                    self.car_list[-1].speed = 0

            self.sprites.update()
            self.screen.fill((255, 255, 255))
            self.sprites.draw(self.screen)
            pygame.display.flip()

    def close(self):
        pygame.quit()


if __name__ == '__main__':
    width, height = 1300, 300
    app = App(width, height)
    app.run()
    app.close()
