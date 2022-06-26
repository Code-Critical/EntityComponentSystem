#include <iostream>
#include <string>

#include "ecs.hpp"

// Type declaration for use as components.
using item_name_type = ecs::tagged_type<std::string, 0>;
using item_stock_type = ecs::tagged_type<unsigned int, 1>;
using item_value_type = ecs::tagged_type<double, 2>;
using item_description_type = ecs::tagged_type<std::string, 3>;
using item_for_sale_type = ecs::tagged_type<bool, 4>;

int main() {
    using namespace ecs;
    
    // Registration of types to allocate memory for components.
    registerComponent<item_name_type>();
    registerComponent<item_stock_type>();
    registerComponent<item_value_type>();
    registerComponent<item_description_type>();
    registerComponent<item_for_sale_type>();

    // Bulk creation of entities.
    for (int i = 0; i < 20; i++) {

        // Registering an entity for use.
        entityHandle entity_id;
        registerEntity(entity_id);

        // Selecting what components we want to enable for this entity.
        enableComponent<item_name_type>(entity_id);
        enableComponent<item_value_type>(entity_id);
        enableComponent<item_description_type>(entity_id);
        enableComponent<item_for_sale_type>(entity_id);

        // Assigning a value to the entity's components.
        setComponent<item_name_type>(entity_id, "Generic Clothing Item " + std::to_string(i));
        setComponent<item_value_type>(entity_id, i * 3);
        setComponent<item_description_type>(entity_id, "Probably made in China.");
        setComponent<item_for_sale_type>(entity_id, i % 3 == 0);
    }

    // Example of implementing a system. Iterator returns a writable reference to the data.
    componentIterator<item_name_type> name_system_accessor;
    for (std::string& name : name_system_accessor) {
        std::cout << name << std::endl;
    }

    componentIterator<item_value_type> value_system_accessor;
    for (auto value : value_system_accessor) {
        std::cout << value << std::endl;
    }

    // Retrieve the value of an existing entity-
    std::string random_access_description;
    if (getComponent<item_description_type>(12, random_access_description)) {
        std::cout << random_access_description << std::endl;
    }

    // If the entity doesn't exist, getComponent will return false and no values will be updated.
    if (getComponent<item_description_type>(55, random_access_description)) {
        std::cout << random_access_description << std::endl;
    }
    
    // Unregistering components to free allocated memory.
    unregisterComponent<item_name_type>();
    unregisterComponent<item_stock_type>();
    unregisterComponent<item_value_type>();
    unregisterComponent<item_description_type>();
    unregisterComponent<item_for_sale_type>();

    return 0;
}   
