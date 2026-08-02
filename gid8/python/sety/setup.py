from setuptools import setup, find_packages

setup(
    name='sety',  # Имя вашего проекта
    version='0.1',  # Версия вашего проекта
    packages=find_packages(),  # Автоматически находит все пакеты
#    py_modules=['shp_ms'],
    install_requires=[
        # Укажите зависимости здесь
        # 'somepackage>=1.0.0',
    ],
    entry_points={
        'console_scripts': [
            'sety=sety.ww:run',  # Имя команды и путь к функции
        ],
    },

 
    author='Sirius',
    author_email='tgid@mail.ru',
    description='Программа для расчета тепловых сетей',
    long_description=open('README.md', encoding='UTF-8').read(),  # Длинное описание из README
    long_description_content_type='text/markdown',
    url='https://github.com/ваш_репозиторий',  # URL вашего проекта
    classifiers=[
        'Programming Language :: Python :: 3',
        'License :: OSI Approved :: MIT License',
        'Operating System :: OS Independent',
    ],
    python_requires='>=3.6',  # Минимальная версия Python
)
