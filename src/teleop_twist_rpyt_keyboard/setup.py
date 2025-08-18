from setuptools import find_packages, setup

package_name = 'teleop_twist_rpyt_keyboard'

setup(
    name=package_name,
    version='0.0.0',
    packages=[],
    py_modules=['teleop_twist_rpyt_keyboard'],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='patrik_ark',
    maintainer_email='pordipatrik@gmail.com',
    description='TODO: Package description',
    license='TODO: License declaration',
    entry_points={
        'console_scripts': [
            'teleop_twist_rpyt_keyboard = teleop_twist_rpyt_keyboard:main'
        ],
    },
)
