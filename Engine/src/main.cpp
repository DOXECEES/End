#include "Window/Window.hpp"
#include "Application.hpp"
#include "Renderer/Renderer.hpp"
#include "Renderer/GdiRenderer/GdiRenderer.hpp"
#include "DockWorkspace.hpp"

#include "Widgets/Button.hpp"

#include <windows.h>
#include <memory>

namespace gui {
    class CounterWidget : public HorizontalLayout {
    public:
        GUI_WIDGET_MOVABLE(CounterWidget)


        CounterWidget(int initialValue = 0) 
            : HorizontalLayout({}), m_value(initialValue) 
        {
            auto minusBtn = std::make_unique<Button>("-");
            auto valueLabel = std::make_unique<Label>(std::to_string(m_value));
            auto plusBtn = std::make_unique<Button>("+");

            m_valueLabel = valueLabel.get();

            minusBtn->onClick([this]() {
                setValue(m_value - 1);
            });

            plusBtn->onClick([this]() {
                setValue(m_value + 1);
            });

            addChild(std::move(minusBtn));
            addChild(std::move(valueLabel));
            addChild(std::move(plusBtn));
        }

        CounterWidget(CounterWidget&&) noexcept = default;
        CounterWidget& operator=(CounterWidget&&) noexcept = default;

        CounterWidget(const CounterWidget&) = delete;
        CounterWidget& operator=(const CounterWidget&) = delete;



        void setValue(int val) noexcept {
            if (m_value != val) {
                m_value = val;
                
                if (m_valueLabel) {
                    m_valueLabel->setText(std::to_string(m_value));
                }

                if (m_onValueChanged) {
                    m_onValueChanged(m_value);
                }

                if (getWindow()) {
                    getWindow()->repaint(true);
                }
            }
        }

        [[nodiscard]] int getValue() const noexcept { 
            return m_value; 
        }

        CounterWidget& onValueChanged(std::function<void(int)> callback) & noexcept 
        {
            m_onValueChanged = std::move(callback);
            return *this;
        }

        CounterWidget&& onValueChanged(std::function<void(int)> callback) && noexcept 
        {
            m_onValueChanged = std::move(callback);
            return std::move(*this);
        }

    private:
        int m_value = 0;
        Label* m_valueLabel = nullptr; 
        std::function<void(int)> m_onValueChanged;
    };
}

int main()
{
    auto nativeRenderer = std::make_shared<Renderer::GdiRenderer>();

    gui::Application app(nativeRenderer); 

    gui::DockWorkspace workspace(1100, 600, "Среда Разработки");

    gui::Window editorWindow(800, 600, "Редактор Кода", &workspace);
    gui::Window propertiesWindow(300, 600, "Свойства", &workspace);
    gui::Window settingWindow(400,500, "Настройки");

    editorWindow.setContent(
        std::make_unique<gui::VerticalLayout>(
            gui::Label("// Файл: main.cpp (Редактор кода)"),

            gui::Label("#include <iostream>"),
            gui::Label("int main() {"),
            gui::Label("    std::cout << \"Hello, World!\" << std::endl;"),
            gui::Label("    return 0;"),
            gui::Label("}"),

            gui::HorizontalLayout(
                gui::Button("Собрать").onClick([]() {
                    MessageBoxW(NULL, L"Компиляция завершена успешно!\nОшибок: 0, Предупреждений: 0", L"Сборка", MB_OK | MB_ICONINFORMATION);
                }).fixedWidth(210),
                gui::Button("Запустить").onClick([]() {
                    MessageBoxW(NULL, L"Вывод программы:\nHello, World!\n\nПроцесс завершился с кодом 0.", L"Запуск", MB_OK);
                }).stretchWidth(3),
                gui::Button("Сохранить").stretchWidth(1)
            )
        )
    );

    propertiesWindow.setContent(
        std::make_unique<gui::VerticalLayout>(
            gui::Label("Настройки сборки проекта"),

            gui::Checkbox("Включить оптимизацию (-O3)").onToggle([](bool checked) {
            }),
            gui::Checkbox("Генерировать debug-информацию (-g)"),
            gui::Checkbox("Использовать стандарт C++23"),
            gui::Checkbox("Предупреждения как ошибки (-Werror)"),

            gui::Button("Применить").onClick([]() {
                MessageBoxW(NULL, L"Настройки проекта успешно применены.", L"Свойства", MB_OK | MB_ICONINFORMATION);
            })
        )
    );

    settingWindow.setContent(
        std::make_unique<gui::VerticalLayout>(
            gui::Label("Выберите количество копий:"),

            gui::CounterWidget(5)
                .onValueChanged([](int newValue) {
                }),

            gui::Button("Печать")
        )
    );


    workspace.dock(&editorWindow, gui::DockAction::Tab);
    workspace.dock(&propertiesWindow, gui::DockAction::Right);
    workspace.dock(&settingWindow, gui::DockAction::Tab);

    app.run();

    return 0;
}