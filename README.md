# cpp-transport-catalogue:round_pushpin:
Финальный проект: транспортный справочник.
Программа поддерживает графический вывод, поиск маршрутов и вычисление времени поездки. 
Программа сможет создать подобную схему маршрутов:
![image](https://user-images.githubusercontent.com/97431581/185622047-948ca5c0-bc3a-4f1d-8b8d-f799e3929fbe.png)

Программная реализация городской системы маршрутизации, работающая с JSON запросами. Программа двухстадийсная и имеет два варианта запросов: первый отвечает за создание базы данных, второй же выполняет всю нужную работу для того, чтобы выдать нужную информацию по любим оптимальным маршрутам, автобусам или остановкам. Форматы вывода: .json и .svg
### Использование
0. Установка и настройка всех требуемых компанентов в среде разработке для запуска приложения.
1. Вариант использования показан в main.cpp 
2. "make_base" - запрос на создание базы данных транспортного каталога (добавление автобусов и остановок).
3. "process_request" - запрос на получение любой информации по остановкам, автобусам или оптимальным маршрутам.
### Системные требования
1. С++17(STL)
2. GCC (MinGW-w64) 11.2.2
### Планы по доработке
1. Добавить карту города со спутника
2. Протестировать реальные координаты и остановки
3. Создать десктопное приложение
### Стек технологий
1. CMake 3.22.0
2. Protobuf-cpp 3.18.1
