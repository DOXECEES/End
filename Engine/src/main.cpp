#include "Window/Window.hpp"
#include "Application.hpp"
#include "Renderer/Renderer.hpp"
#include "Renderer/GdiRenderer/GdiRenderer.hpp"
#include "DockWorkspace.hpp"

#include "Widgets/Button.hpp"
#include "Widgets/Label.hpp"
#include "Widgets/Checkbox.hpp"
#include "Widgets/Separator.hpp"
#include "Widgets/Image.hpp"
#include "Widgets/TextBox.hpp"
#include "Widgets/ComboBox.hpp"
#include "Widgets/HorizontalLayout.hpp"

#include <windows.h>
#include <memory>
#include <algorithm>

namespace gui {

    constexpr int ROW      = 34;
    constexpr int TITLE    = 26;
    constexpr int HINT     = 18;
    constexpr int DIVIDER  = 2;
    constexpr int GAP      = 10;

    inline Label Spacer(int h = GAP) {
        Label lbl("");
        lbl.fixedHeight(h);
        return lbl;
    }

    class CounterWidget : public HorizontalLayout {
    public:
        GUI_WIDGET_MOVABLE(CounterWidget)
        REGISTER_WIDGET_CLASS_NAME(CounterWidget)

        CounterWidget(int initialValue = 0, int minValue = 0, int maxValue = 999999)
            : HorizontalLayout({}), m_value(initialValue), m_min(minValue), m_max(maxValue)
        {
            auto minusBtn = std::make_unique<Button>("-");
            auto valueLabel = std::make_unique<Label>(std::to_string(m_value));
            auto plusBtn = std::make_unique<Button>("+");

            m_valueLabel = valueLabel.get();

            minusBtn->onClick([this]() { setValue(m_value - 1); });
            plusBtn->onClick([this]() { setValue(m_value + 1); });

            minusBtn->addClass("btn-mini");
            plusBtn->addClass("btn-mini");
            minusBtn->fixedWidth(30);
            plusBtn->fixedWidth(30);
            valueLabel->addClass("mono-value");

            addChild(std::move(minusBtn));
            addChild(std::move(valueLabel));
            addChild(std::move(plusBtn));
        }

        CounterWidget(CounterWidget&&) noexcept = default;
        CounterWidget& operator=(CounterWidget&&) noexcept = default;
        CounterWidget(const CounterWidget&) = delete;
        CounterWidget& operator=(const CounterWidget&) = delete;

        void setValue(int val) noexcept {
            val = std::clamp(val, m_min, m_max);
            if (m_value != val) {
                m_value = val;
                if (m_valueLabel) m_valueLabel->setText(std::to_string(m_value));
                if (m_onValueChanged) m_onValueChanged(m_value);
                if (getWindow()) getWindow()->repaint(true);
            }
        }

        [[nodiscard]] int getValue() const noexcept { return m_value; }

        CounterWidget& onValueChanged(std::function<void(int)> callback) & noexcept {
            m_onValueChanged = std::move(callback);
            return *this;
        }
        CounterWidget&& onValueChanged(std::function<void(int)> callback) && noexcept {
            m_onValueChanged = std::move(callback);
            return std::move(*this);
        }

    private:
        int m_value = 0;
        int m_min = 0;
        int m_max = 999999;
        Label* m_valueLabel = nullptr;
        std::function<void(int)> m_onValueChanged;
    };

    template <typename ControlT>
    HorizontalLayout FormRow(const std::string& label, ControlT&& control, int labelWidth = 190) {
        HorizontalLayout row(
            Label(label).addClass("hint").fixedWidth(labelWidth),
            std::forward<ControlT>(control)
        );
        row.fixedHeight(ROW);
        return row;
    }

    template <typename... Rows>
    VerticalLayout Card(const std::string& title, Rows&&... rows)  {
        VerticalLayout card(
            Label(title).addClass("card-title").fixedHeight(TITLE),
            std::forward<Rows>(rows)...
        );

        constexpr int numRows = sizeof...(rows);
        constexpr int numChildren = numRows + 1; 
        
        int totalGaps = (numChildren - 1) * 12;
        int totalPadding = 16 * 2;
        
        int calculatedHeight = TITLE + (numRows * ROW) + totalGaps + totalPadding;

        card.fixedHeight(calculatedHeight);
        card.addClass("card");
        return card;
    }

    inline Label SeasonSeal(const std::string& seasonClass) {
        Label seal("");
        seal.fixedWidth(16);
        seal.addClass("seal");
        seal.addClass(seasonClass);
        return seal;
    }
}

int main()
{
    auto nativeRenderer = std::make_shared<Renderer::GdiRenderer>();
    gui::Application app(nativeRenderer);

    gui::StyleSheet ledgerTheme = {
        { "Window", {
            { "background-color", "rgb(18,20,15)" },
            { "font-family",      "'PT Serif', 'Segoe UI', serif" },
            { "font-size",        "14px" }
        }},

        { "VerticalLayout", {
            { "gap", "12px" }
        }},
        { "HorizontalLayout", {
            { "gap", "12px" }
        }},

        { "Button", {
            { "background-color", "rgb(31,36,25)" },
            { "border",           "1px solid rgb(46,53,39)" },
            { "text-color",       "rgb(233,228,212)" },
            { "border-radius",    "6px" },
            { "padding",          "6px 16px" },
            { "font-weight",      "600" },
            { "cursor",           "pointer" },
            { "transition",       "background-color 0.15s ease, border-color 0.15s, box-shadow 0.15s" }
        }},
        { "Button:hover", {
            { "background-color", "rgb(42,48,34)" },
            { "border-color",     "rgb(138,100,35)" },
            { "box-shadow",       "0px 4px 12px rgba(0, 0, 0, 0.25)" }
        }},
        { "Button:pressed", {
            { "background-color", "rgb(201,138,44)" },
            { "border-color",     "rgb(201,138,44)" },
            { "text-color",       "rgb(32,23,3)" },
            { "transform",        "scale(0.98)" }
        }},

        { "btn-primary", {
            { "background-color", "rgb(201,138,44)" },
            { "border-color",     "rgb(201,138,44)" },
            { "text-color",       "rgb(32,23,3)" }
        }},
        { "btn-primary:hover", {
            { "background-color", "rgb(227,168,63)" },
            { "box-shadow",       "0px 4px 14px rgba(201, 138, 44, 0.2)" }
        }},

        
        { "Label", {
            { "text-color", "rgb(233,228,212)" }
        }},
        { "brand-title", {
            { "font-size",   "22px" },
            { "font-weight", "700" },
            { "text-color",  "rgb(233,228,212)" }
        }},
        { "brand-eyebrow", {
            { "font-size",      "11px" },
            { "font-weight",    "800" },
            { "text-color",     "rgb(201,138,44)" },
            { "letter-spacing", "1.5px" },
            { "text-transform", "uppercase" }
        }},
        { "card-title", {
            { "font-size",      "16px" },
            { "font-weight",    "700" },
            { "text-color",     "rgb(227,168,63)" },
            { "margin-bottom",  "8px" }
        }},
        { "hint", {
            { "text-color", "rgb(183,192,168)" },
            { "font-size",  "13px" }
        }},
        { "muted", {
            { "text-color", "rgb(124,133,112)" },
            { "font-size",  "12px" }
        }},
        { "mono-value", {
            { "font-family", "Consolas, monospace" },
            { "font-weight", "600" }
        }},

        
        { "TextBox", {
            { "background-color", "rgb(23,27,18)" },
            { "border",           "1px solid rgb(46,53,39)" },
            { "text-color",       "rgb(233,228,212)" },
            { "border-radius",    "4px" },
            { "padding",          "6px 12px" },
            { "transition",       "border-color 0.15s, box-shadow 0.15s" }
        }},
        { "TextBox:focused", {
            { "border-color", "rgb(138,100,35)" },
            { "box-shadow",   "0px 0px 0px 3px rgba(138, 100, 35, 0.15)" }
        }},
        { "ComboBox", {
            { "background-color", "rgb(23,27,18)" },
            { "border",           "1px solid rgb(46,53,39)" },
            { "text-color",       "rgb(233,228,212)" },
            { "border-radius",    "4px" },
            { "padding",          "6px 12px" }
        }},

         { "ComboBoxItem", {
            { "background-color", "rgb(23,27,18)" },     
            { "text-color",       "rgb(183,192,168)" },    
            { "padding",          "6px 12px" }
        }},
        { "ComboBoxItem:hover", {
            { "background-color", "rgb(201,138,44)" },    
            { "text-color",       "rgb(32,23,3)" }         
        }},
        { "dropdown-layout", {
            { "gap", "0px" }
        }},

        { "card", {
            { "background-color", "rgb(31,36,25)" },
            { "border",           "1px solid rgb(46,53,39)" },
            { "border-radius",    "8px" },
            { "padding",          "16px" },
            { "box-shadow",       "0px 8px 24px rgba(0, 0, 0, 0.2)" }
        }},

        { "Separator", {
            { "background-color", "rgb(46,53,39)" },
            { "height",           "1px" },
            { "margin",           "8px 0px" }
        }},
        { "seal", {
            { "width",         "16px" },
            { "height",        "16px" },
            { "border-radius", "50%" },
            { "margin-right",  "8px" }
        }},
        { "seal-spring", { { "background-color", "rgb(143,174,93)" } }},
        { "seal-summer", { { "background-color", "rgb(217,166,46)" } }},
        { "seal-fall",   { { "background-color", "rgb(193,104,46)" } }},
        { "seal-winter", { { "background-color", "rgb(107,150,168)" } }}
    };

    app.setStyleSheet(std::move(ledgerTheme));

    gui::DockWorkspace workspace(1400, 800, "Гроссбух фермера — Save Editor v1.7");

    gui::Window sidebarWindow(300, 800, "Гроссбух", &workspace);
    gui::Window farmWindow(760, 800, "Ферма", &workspace);

    using namespace gui;

    sidebarWindow.setContent(
        std::make_unique<VerticalLayout>(
            Label("РЕДАКТОР СОХРАНЕНИЙ · v1.7").addClass("brand-eyebrow").fixedHeight(HINT),
            Label("Гроссбух фермера").addClass("brand-title").fixedHeight(TITLE + 4),
            Label("Stardew Valley Save Editor").addClass("muted").fixedHeight(HINT),
            Spacer(),

            Separator().fixedHeight(DIVIDER),
            Spacer(GAP / 2),

            HorizontalLayout(
                Button("📂 Открыть").onClick([]() {}).stretchWidth(1),
                Button("💾 Сохранить").onClick([]() {}).addClass("btn-primary").stretchWidth(1)
            ).fixedHeight(ROW),
            Spacer(GAP / 2),
            Button("🗄 Резервная копия (.bak)").onClick([]() {}).fixedHeight(ROW),

            Spacer(),
            Label("ПЕРСОНАЖ").addClass("muted").fixedHeight(HINT),
            Image("C:/Repos/End/Engine/assets/player_placeholder.png")
                .fixedHeight(150)
                .addClass("card"),

            Spacer(),
            Label("ТЕКУЩИЙ ФАЙЛ").addClass("muted").fixedHeight(HINT),
            VerticalLayout(
                HorizontalLayout(
                    Label("👤 Имя").addClass("hint").stretchWidth(1),
                    Label("Farmer John").addClass("mono-value").stretchWidth(1)
                ).fixedHeight(HINT + 6),
                HorizontalLayout(
                    Label("🏡 Ферма").addClass("hint").stretchWidth(1),
                    Label("Sunny Meadow").addClass("mono-value").stretchWidth(1)
                ).fixedHeight(HINT + 6),
                HorizontalLayout(
                    Label("📅 Дата").addClass("hint").stretchWidth(1),
                    Label("14 Весны, Y2").addClass("mono-value").stretchWidth(1)
                ).fixedHeight(HINT + 6),
                HorizontalLayout(
                    Label("⏱ Время").addClass("hint").stretchWidth(1),
                    Label("122:40").addClass("mono-value").stretchWidth(1)
                ).fixedHeight(HINT + 6)
            ).fixedHeight((HINT + 6) * 4).addClass("card"),

            Label("").stretchHeight(1),

            Separator().fixedHeight(DIVIDER),
            HorizontalLayout(
                Label("●").addClass("status-dot").fixedWidth(20),
                Label("Есть несохранённые изменения").addClass("muted").stretchWidth(1)
            ).fixedHeight(ROW)
        )
    );

    farmWindow.setContent(
        std::make_unique<VerticalLayout>(
            Label("Общие настройки фермы").addClass("brand-title").fixedHeight(TITLE),
            Label("Идентификация усадьбы, точное время, экономика и глобальные вехи прогресса").addClass("muted").fixedHeight(HINT),
            Spacer(),

            HorizontalLayout(
                Card("Усадьба и профиль",
                    FormRow("Имя фермера", TextBox("Farmer John").stretchWidth(1), 150),
                    FormRow("Название фермы", TextBox("Sunny Meadow").stretchWidth(1), 150),
                    FormRow("Тип усадьбы", ComboBox(
                        std::vector<std::string>{"Стандартная", "Речная долина", "Лесная", "Холмистая", "Дикая", "Четыре угла", "Прибрежная", "Луга (1.6)"}
                    ).stretchWidth(1), 150),
                    FormRow("Любимое дело", TextBox("C++").stretchWidth(1), 150),
                    FormRow("Пещера фермы", ComboBox(
                        std::vector<std::string>{"Не выбрано", "Плодовые летучие мыши", "Грибы"}
                    ).stretchWidth(1), 150)
                ).stretchWidth(1),

                Card("Календарь и время",
                    HorizontalLayout(
                        Label("Сезон").addClass("hint").fixedWidth(114),
                        SeasonSeal("seal-spring"),
                        ComboBox(
                            std::vector<std::string>{"Весна (Spring)", "Лето (Summer)", "Осень (Fall)", "Зима (Winter)"}
                        ).stretchWidth(1)
                    ).fixedHeight(ROW),
                    FormRow("День / Год", HorizontalLayout(
                        CounterWidget(14, 1, 28).stretchWidth(1),
                        CounterWidget(2, 1, 99).stretchWidth(1)
                    ), 130),
                    FormRow("Игровое время", CounterWidget(600, 600, 2600).stretchWidth(1), 130),
                    FormRow("Погода сегодня", ComboBox(
                        std::vector<std::string>{"Солнечно", "Облачно", "Дождь", "Гроза", "Снег", "Фестиваль"}
                    ).stretchWidth(1), 130),
                    FormRow("Погода завтра", ComboBox(
                        std::vector<std::string>{"Солнечно", "Облачно", "Дождь", "Гроза", "Снег", "Фестиваль"}
                    ).stretchWidth(1), 130)
                ).stretchWidth(1)
            ).fixedHeight(288), 

            Spacer(),

            HorizontalLayout(
                Card("Финансы и валюта",
                    FormRow("Золото (g)", CounterWidget(15700, 0, 999999999).stretchWidth(1), 150),
                    FormRow("Всего заработано", CounterWidget(45000, 0, 999999999).stretchWidth(1), 150),
                    FormRow("Монеты Ки", CounterWidget(250, 0, 99999).stretchWidth(1), 150),
                    FormRow("Драгоценные камни Ки", CounterWidget(15, 0, 9999).stretchWidth(1), 150),
                    FormRow("Золотые орехи", CounterWidget(42, 0, 130).stretchWidth(1), 150)
                ).stretchWidth(1),

                Card("Дом и питомец",
                    FormRow("Уровень дома", ComboBox(
                        std::vector<std::string>{"Без улучшений (Ур. 0)", "Кухня (Ур. 1)", "Детская (Ур. 2)", "Подвал (Ур. 3)"}
                    ).stretchWidth(1), 130),
                    FormRow("Имя питомца", TextBox("Dusty").stretchWidth(1), 130),
                    FormRow("Тип питомца", ComboBox(
                        std::vector<std::string>{"Кот (Рыжий)", "Кот (Серый)", "Кот (Черный)", "Собака (Овчарка)", "Собака (Такса)", "Черепаха"}
                    ).stretchWidth(1), 130),
                    FormRow("Дружба питомца", CounterWidget(1000, 0, 1000).stretchWidth(1), 130),
                    FormRow("Теплица у дома", ComboBox(
                        std::vector<std::string>{"Закрыта", "Разблокирована / Доступна"}
                    ).stretchWidth(1), 130)
                ).stretchWidth(1)
            ).fixedHeight(288), 

            Spacer(),

            Card("Глобальный прогресс и ключевые предметы",
                HorizontalLayout(
                    Checkbox("Восстановить Общественный центр (Клуб)").stretchWidth(1),
                    Checkbox("Приобрести членство корпорации Joja").stretchWidth(1),
                    Checkbox("Получить ржавый ключ от канализации").stretchWidth(1)
                ).fixedHeight(ROW),
                HorizontalLayout(
                    Checkbox("Получить ключ скелета для Пещеры Черепа").stretchWidth(1),
                    Checkbox("Получить клубную карту Ки").stretchWidth(1),
                    Checkbox("Разблокировать Золотую косу").stretchWidth(1)
                ).fixedHeight(ROW)
            )
        )
    );

    workspace.dock(&sidebarWindow, DockAction::Tab);
    workspace.dock(&farmWindow, DockAction::Tab);

    app.run();
    return 0;
}