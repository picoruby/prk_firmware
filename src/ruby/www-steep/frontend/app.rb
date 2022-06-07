require 'ovto'

class MyApp < Ovto::App
  class State < Ovto::State
    item :keymap, default: ""
    item :result, default: ""
  end

  class Actions < Ovto::Actions
    def update_keymap(state:, keymap:)
      { keymap: keymap }
    end

    def steep_check(state:, button:)
      return if state.keymap.length == 0
      button.disabled = true
      Ovto.fetch('http://localhost:4000/offline/steep', 'POST', {keymap: state.keymap}).then{ |json|
        actions.receive_action(json: json)
        button.disabled = false
      }.fail{ |e|  # Network error, 404 Not Found, JSON parse error, etc.
        p e
        button.disabled = false
      }
    end

    def receive_action(json:, error: false)
      { result: json["message"] }
    end
  end

  class MainComponent < Ovto::Component
    def render(state:)
      o 'div', class: "px-10 py-4" do
        o 'div', class: "my-4" do
          o 'textarea', {
            placeholder: "kbd = Keyboard.new",
            class: "border-solid border-2 w-full h-72 p-4 font-mono whitespace-nowrap",
            value: state.keymap,
            onchange: ->(e){ actions.update_keymap(keymap: e.target.value) }
          }
        end
        o 'div', class: "my-4" do
          o 'button', class: "bg-red-700 font-semibold text-white py-2 px-10 rounded", onclick: ->(e){ actions.steep_check(button: e.target) } do
            'Check'
          end
        end
        o 'div', class: "my-4" do
          o 'div', 'Result:'
          o 'div', class: "bg-slate-400 font-mono leading-none p-4" do
            o 'code' do
              state.result.split("\n").each do |line|
                o 'text', line
                o 'br'
              end
            end
          end
        end
      end
    end
  end
end

MyApp.run(id: 'ovto')

