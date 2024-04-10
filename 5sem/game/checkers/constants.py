from checkers.point import Point
from checkers.enums import CheckerType, SideType

# Сторона за которую играет игрок
PLAYER_SIDE = SideType.WHITE

# Размер поля
X_SIZE = Y_SIZE = 8
# Размер ячейки (в пикселях)
CELL_SIZE = 100

# Скорость анимации (больше = быстрее)
ANIMATION_SPEED = 8

# Количество ходов для предсказания
MAX_PREDICTION_DEPTH = 3

# Ширина рамки (Желательно должна быть чётной)
BORDER_WIDTH = 2 * 2

# Цвета игровой доски
FIELD_COLORS = ['#ffffff', '#000000']
# Цвет рамки при наведении на ячейку мышкой
HOVER_BORDER_COLOR = '#000000'
# Цвет рамки при выделении ячейки
SELECT_BORDER_COLOR = '#44cc1b'
# Цвет кружков возможных ходов
POSIBLE_MOVE_CIRCLE_COLOR = '#44cc1b'

# Возможные смещения ходов шашек
MOVE_OFFSETS = [
    Point(-1, -1),
    Point( 1, -1),
    Point(-1,  1),
    Point( 1,  1)
]

# Массивы типов белых и чёрных шашек [Обычная пешка, дамка]
WHITE_CHECKERS = [CheckerType.WHITE_REGULAR, CheckerType.WHITE_QUEEN]
BLACK_CHECKERS = [CheckerType.BLACK_REGULAR, CheckerType.BLACK_QUEEN]