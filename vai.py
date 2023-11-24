import pygame as pg
import sys
from pygame.locals import *

pg.init()
vec = pg.math.Vector2  # 2 for two dimensional

data = [
    117830, 117820, 117835, 117865, 117850,
    117855, 117900, 117890, 117905, 117920,
    117925, 117980, 117975, 118010, 118030,
    118075, 118165, 118180, 118175, 118105,
    118045, 118030, 118005, 117990, 118015,
    118010, 118035, 118060, 118045, 118075,
    118075, 118055, 118030, 118040, 118040,
    118020, 118095, 118140, 118140, 118135,
    118125, 118075, 118050, 118030, 118105,
    118160, 118195, 118195, 118190, 118160,
    118195, 118220, 118220, 118210, 118210,
    118205, 118185, 118185, 118195, 118195,
    118170, 118175, 118175, 118185, 118175,
    118185, 118210, 118195, 118200, 118210,
    118210, 118195, 118195, 118175, 118180,
    118190, 118195, 118190, 118190, 118195,
    118200, 118190, 118260, 118260, 118260,
    118245, 118270, 118270, 118270, 118295,
    118290, 118260, 118255, 118235, 118275,
    118280, 118260, 118275, 118310, 118305
]

HEIGHT = 1000
WIDTH = 1000
FPS = 60
SCALE_UP = 1.2
SCALE_DOWN = 0.8

FramePerSec = pg.time.Clock()

displaysurface = pg.display.set_mode((WIDTH, HEIGHT))
pg.display.set_caption("Game")


class DataPoint(pg.sprite.Sprite):
    def __init__(self, x, y):
        super().__init__()
        self.surf = pg.Surface((30, 30))
        self.surf.fill((128,255,40))
        self.rect = self.surf.get_rect()

        self.pos = vec((x, y))


class GameState:
    def __init__(self):
        self.zoom = 1
        self.world_offset_x = 0
        self.world_offset_y = 0
        self.update_screen = True
        self.panning = False
        self.pan_start_pos = None
        self.legacy_screen = pg.Surface((WIDTH, HEIGHT))

game_state = GameState()

def world_2_screen(world_x, world_y):
    screen_x = (world_x - game_state.world_offset_x) * game_state.zoom
    screen_y = (world_y - game_state.world_offset_y) * game_state.zoom
    return [screen_x, screen_y]


def screen_2_world(screen_x, screen_y):
    world_x = (screen_x / game_state.zoom) + game_state.world_offset_x
    world_y = (screen_y / game_state.zoom) + game_state.world_offset_y
    return [world_x, world_y]



data_points = list(map(lambda d: DataPoint(d[0], d[1]), zip(range(len(data)), data)))

all_sprites = pg.sprite.Group()
all_sprites.add(data_points)

while True:
    pg.display.set_caption('(%d FPS)' % (FramePerSec.get_fps()))

    mouse_x, mouse_y = pg.mouse.get_pos()

    for event in pg.event.get():
        if event.type == QUIT:
            pg.quit()
            sys.exit()

        elif event.type == pg.MOUSEBUTTONDOWN:
            if event.button == 4 or event.button == 5:
                # X and Y before the zoom
                mouseworld_x_before, mouseworld_y_before = screen_2_world(mouse_x, mouse_y)

                # ZOOM IN/OUT
                if event.button == 4 and game_state.zoom < 10:
                    game_state.zoom *= SCALE_UP
                elif event.button == 5 and game_state.zoom > 0.5:
                    game_state.zoom *= SCALE_DOWN

                # X and Y after the zoom
                mouseworld_x_after, mouseworld_y_after = screen_2_world(mouse_x, mouse_y)

                # Do the difference between before and after, and add it to the offset
                game_state.world_offset_x += mouseworld_x_before - mouseworld_x_after
                game_state.world_offset_y += mouseworld_y_before - mouseworld_y_after

            elif event.button == 1:
                # PAN START
                game_state.panning = True
                game_state.pan_start_pos = mouse_x, mouse_y

        elif event.type == pg.MOUSEBUTTONUP:
            if event.button == 1 and game_state.panning:
                # PAN STOP
                game_state.panning = False


    if game_state.panning:
        # Pans the screen if the left mouse button is held
        game_state.world_offset_x -= (mouse_x - game_state.pan_start_pos[0]) / game_state.zoom
        game_state.world_offset_y -= (mouse_y - game_state.pan_start_pos[1]) / game_state.zoom
        game_state.pan_start_pos = mouse_x, mouse_y

    if game_state.update_screen:

        for entity in all_sprites:
            displaysurface.blit(entity.surf, entity.rect)
        # Updates the legacy screen if something has changed in the image data
        game_state.update_screen = False

    # Sets variables for the section of the legacy screen to be zoomed
    world_left, world_top = screen_2_world(0, 0)
    world_right, world_bottom = WIDTH/game_state.zoom, HEIGHT/game_state.zoom

    # Makes a temp surface with the dimensions of a smaller section of the legacy screen (for zooming).
    new_screen = pg.Surface((world_right, world_bottom))
    # Blits the smaller section of the legacy screen to the temp screen
    new_screen.blit(game_state.legacy_screen, (0, 0), (world_left, world_top, world_right+1, world_bottom+1))
    # Blits the final cut-out to the main screen, and scales the image to fit with the screen height and width
    displaysurface.fill((0, 0, 0))
    displaysurface.blit(pg.transform.scale(new_screen, (int(WIDTH+game_state.zoom), int(HEIGHT+game_state.zoom))), (-(world_left%1)*game_state.zoom, -(world_top%1)*game_state.zoom))

    pg.display.update()
    FramePerSec.tick(FPS)
