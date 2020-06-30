## ECS

ECS (Entity–component–system) is an architectural model that CLUSEK is based on. ECS follows the composition over inheritance principle that allows greater flexibility in defining entities where every object in a game's scene is an entity.

<img src="./ecs_diagram.png" >

### Entities

We can visualize entities in our heads as objects on the scene. For example, a vehicle on the scene can be an entity, a house can be an entity and terrain can be an entity.

We can add new entities from code or from the map file. In 99.9% of cases, objects are added from the map file. Example entity for the `Main Camera`:

```
[
  {
    "Tags":[
      "Main Camera"
    ],
    "Components":{
      "CameraComponent":{

      },
      "TransformComponent":{
        "PositionX":-0.9,
        "PositionY":1.5,
        "PositionZ":2.25
      }
    }
  }
]
```

### Components

Components store information about the current state of an entity. One entity can have many components that store information about different things in the object.

Adding a new component is quite complicated and will be described here. First of all new component has to be added as a struct in the `ECS/Components` filter and folder. Example component:

```
struct ExampleComponent final
{
	float a;
	float b;
	int c;
};
```


Loader for this component is required. It has to be added and placed in the `Loaders/Components` filter and folder. Naming for this loader should be `*Loader`, where `*` is the component name, due to the special macro. Example loader definition:

```
#pragma once
#include "BaseComponentsLoader.h"

class ExampleComponentLoader final : public BaseComponentsLoader
{
public:
	void Add(nlohmann::json& json, entt::registry& registry, const entt::registry::entity_type &entity) override;
};
```

This class of course need to be implemented. For easier implementation special macro has been designed for mapping fields. Example component loader implementation:

```
#include "ExampleComponentLoader.h"

void ExampleComponentLoader::Add(nlohmann::json& json, entt::registry& registry, const entt::registry::entity_type& entity)
{
	auto &component = registry.assign<ExampleComponent>(entity);

	MAP_LOADER_BASIC_FIELD(a, json, component, float);
	MAP_LOADER_BASIC_FIELD(b, json, component, float);
	MAP_LOADER_BASIC_FIELD(c, json, component, int);
}
```

To register out loader we have to declare it in `MapLoader.h` file. To make it work we have to register it with a name ending with `*Instance`. Example:

```
inline static ExampleComponentLoader ExampleComponentLoaderInstance;
```

And now, all we have to do is add this to the `MapLoader::AddComponents` function, like for example this:

```
MAP_COMPONENT_LOADERS(it, ExampleComponent, registry, entity);
```

At this point, our component can be added from the map file and is fully functional, but it will not be available in the editor. To make it work we have to perform extra steps.

First of all, we have to add our component to the entity list to make it selectable from the `EntityViewer`. To do this we have to modify `EntityViewerWindow::DrawList` in the `EntityViewerWindow.cpp` by registering a new component with `EntityViewerWindow::DrawComponent` private function. Example:

```
DrawComponent<ExampleComponent>(entity, "Example Component");
```

To make `ExampleComponent` editable in real-time from UI we have to add a special class. This class needs to be named `*Editor`, where `*` is a component name. A class should extend `BaseComponentEditor<T>`, where `T` is out component class and places in `GUI/Components` filter and folder. Example:

```
#pragma once
#include "BaseComponentEditor.h"
#include "../../ECS/Components/ExampleComponent.h"

class ExampleComponentEditor final : public BaseComponentEditor<ExampleComponent>
{
protected:
	void Draw() override;
};
```

`ExampleComponentEditor::Draw` function can be overridden, but it is not required. Implementation of this function gives access to fields in the entity. An example implementation of this function:

```
#include "ExampleComponentEditor.h"

void ExampleComponentEditor::Draw()
{
	const auto component = GetPointerToThisComponent();

	ImGui::InputFloat("A", &component->a);
	ImGui::InputFloat("B", &component->b);
	ImGui::InputInt("C", &component->c);
}
```

To register component editor we have to create a new instance of it in `EntityEditorWindow.h` file and name its instance proper name ending with `*Instance`, where `*` is component editor name. Example:

```
ExampleComponent ExampleComponentEditorInstance;
```

To finish our process of adding a new component, we have to just register our editor in the `` file. Example:

```
REGISTER_COMPONENT_EDITOR(ExampleComponent, "Example Component");
```

### Systems

Systems are logic holders in this game engine architecture. Each system can perform any action with any entity and underlying components. Systems have access not only to entities but also to whole engine logic and architecture to perform logic operations.

Adding a new system is simple. We need to create a new system class and place it in the correct filter and folder, which is `ECS/Systems`. Example system class declaration:

```
#pragma once
#include "BaseSystem.h"

class ExampleSystem final : public BaseSystem
{
public:
	void Start() override;
	void Update(float deltaTime) override;
};
```

Next, it needs to be implemented, but implementation depends on the use case, so it will not be shown here.

The last step is registering our system in the `Engine.cpp`. To do that we need to call a special register function in the `Engine::CreateSystems` function. Example of registering new system:

```
// System A, B, C, D
RegisterNewSystem<ExampleSystem>("Example System");
// System E, F, G, H
```