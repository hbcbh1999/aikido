import r3py

PACKAGE_NAME = 'herb_description'
PACKAGE_PATH = '/home/parallels/ros-herb/src/herb_description'
URDF_PATH \
    = '/home/parallels/ros-herb/devel/share/herb_description/robots/herb.urdf'

urdf_loader = r3py.DartLoader()
urdf_loader.add_package_directory(PACKAGE_NAME, PACKAGE_PATH)
robot = urdf_loader.parse_skeleton(URDF_PATH)

world = r3py.World()
robot_name = world.add_skeleton(robot)

window = r3py.SimWindow()
window.world = world
window.init_window(1600, 1200, 'HERB')
window.spin()
