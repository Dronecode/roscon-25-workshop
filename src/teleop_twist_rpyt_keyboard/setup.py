from setuptools import setup

package_name = 'teleop_twist_rpyt_keyboard'

setup(
    name=package_name,
    version='0.0.0',
    py_modules=[package_name.replace('-', '_')],  # use module name
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='patrik_ark',
    maintainer_email='pordipatrik@gmail.com',
    description='TODO: Package description',
    license='TODO: License declaration',
    entry_points={
        'console_scripts': [
            'teleop_twist_rpyt_keyboard = teleop_twist_rpyt_keyboard:main',
        ],
    },
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
)
