#pragma once
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/string.hpp>
#include <unordered_map>
#include "i_card_database.h"
#include "card_data.h"

namespace godot {

/// 카드 데이터베이스 싱글턴 Node
/// .tres 파일을 로드하여 C++ CardData로 변환하고 ICardDatabase를 구현한다.
class GDCardDatabase : public Node, public ICardDatabase {
    GDCLASS(GDCardDatabase, Node)
public:
    GDCardDatabase();
    ~GDCardDatabase();

    static GDCardDatabase* get_singleton();

    void _ready() override;

    // ICardDatabase
    const CardData* get_card(const std::string& id) const override;

    // GDScript → C++ 역방향 조회 (손패 표시에 사용)
    Ref<Resource> get_gd_resource(const std::string& id) const;

protected:
    static void _bind_methods();

private:
    static GDCardDatabase* _singleton;

    std::unordered_map<std::string, CardData>      _cpp_cards;
    std::unordered_map<std::string, Ref<Resource>> _gd_resources;

    void     _load_cards();
    CardData _convert_card_resource(const Ref<Resource>& res) const;
};

} // namespace godot
