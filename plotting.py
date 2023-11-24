import pygame
import math
import sys


class TimeCluster:
    SCALE_UP = 1.25
    SCALE_DOWN = 0.8

    def run(self, series):
        pygame.init()
        pygame.display.set_caption("TimeCluster")

        self.screen = pygame.display.set_mode((1920, 1080), pygame.RESIZABLE)
        self.clock = pygame.time.Clock()

        self.font = pygame.font.Font("resources/consola.ttf", 50)

        self.series = series

        self.zoom = 1
        self.world_offset_x = 0
        self.world_offset_y = 0

        self.legacy_screen = pygame.Surface((1920, 1080))

        i = 0
        for close in self.series:
            self._draw_point(i, close)
            i += 1

        while True:
            self._loop()

    def _loop(self):
        self._process_events()
        self._draw()

        pygame.display.update()
        self.clock.tick(100)

    def _draw(self):
        self.screen.fill((0, 0, 0))

        world_left, world_top = self._screen_2_world(0, 0)
        world_right, world_bottom = (
            self.screen.get_width() / self.zoom,
            self.screen.get_height() / self.zoom,
        )

        new_screen = pygame.Surface((world_right, world_bottom))
        new_screen.blit(
            self.legacy_screen,
            (0, 0),
            (world_left, world_top, world_right + 1, world_bottom + 1),
        )
        self.screen.blit(
            pygame.transform.scale(
                new_screen,
                (
                    int(self.screen.get_width() + self.zoom),
                    int(self.screen.get_height() + self.zoom),
                ),
            ),
            (-(world_left % 1) * self.zoom, -(world_top % 1) * self.zoom),
        )

        fps_counter = self.font.render(
            f"{math.floor(self.clock.get_fps())}", True, "White"
        )
        self.screen.blit(fps_counter, (self.screen.get_width() - 100, 10))

    def _draw_point(self, x, y):
        pygame.draw.circle(
            self.legacy_screen,
            (255, 0, 0),
            (x * 10 + 10, self.screen.get_height() - y),
            2,
        )

    def _process_events(self):
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                self._quit()

            if event.type == pygame.KEYDOWN:
                self._process_key_down(event)

            elif event.type == pygame.MOUSEBUTTONDOWN:
                self._process_mouse_button_down(event)

            if event.type == pygame.VIDEORESIZE:
                self.screen = pygame.display.set_mode(
                    (event.w, event.h), pygame.RESIZABLE
                )

    def _process_mouse_button_down(self, event):
        if event.button == 4 or event.button == 5:
            self._camera_zoom(event)

    def _camera_zoom(self, event):
        # X and Y before the zoom
        mouse_x, mouse_y = pygame.mouse.get_pos()
        mouseworld_x_before, mouseworld_y_before = self._screen_2_world(
            mouse_x, mouse_y
        )

        # ZOOM IN/OUT
        if event.button == 4 and self.zoom < 10:
            self.zoom *= self.SCALE_UP
        elif event.button == 5 and self.zoom > 0.5:
            self.zoom *= self.SCALE_DOWN

        # X and Y after the zoom
        mouseworld_x_after, mouseworld_y_after = self._screen_2_world(mouse_x, mouse_y)

        # Do the difference between before and after, and add it to the offset
        self.world_offset_x += mouseworld_x_before - mouseworld_x_after
        self.world_offset_y += mouseworld_y_before - mouseworld_y_after

    def _process_key_down(self, event):
        if event.key == pygame.K_ESCAPE:
            self._quit()

    def _quit(self):
        pygame.quit()
        sys.exit()

    def _world_2_screen(self, world_x, world_y):
        screen_x = (world_x - self.world_offset_x) * self.zoom
        screen_y = (world_y - self.world_offset_y) * self.zoom
        return [screen_x, screen_y]

    def _screen_2_world(self, screen_x, screen_y):
        world_x = (screen_x / self.zoom) + self.world_offset_x
        world_y = (screen_y / self.zoom) + self.world_offset_y
        return [world_x, world_y]


def run(series):
    TimeCluster().run(series)
