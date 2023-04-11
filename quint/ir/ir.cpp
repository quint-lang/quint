//
// Created by BY210033 on 2023/3/10.
//
#include "quint/ir/ir.h"
#include "quint/analysis/analysis.h"
#include "quint/ir/transforms.h"
#include "quint/ir/statements.h"
//#include "quint/program/compile_config.h"

namespace quint::lang {

    std::string Identifier::raw_name() const {
        if (name_.empty())
            return fmt::format("tmp{}", id);
        return name_;
    }

    Stmt *VecStatement::push_back(pStmt &&stmt) {
        auto ret = stmt.get();
        stmts.push_back(std::move(stmt));
        return ret;
    }

    IRNode *IRNode::get_ir_root() {
        auto node = this;
        while (node->get_parent()) {
            node = node->get_parent();
        }
        return node;
    }

    std::unique_ptr<IRNode> IRNode::clone() {
        std::unique_ptr<IRNode> new_irnode;
        if (is<Block>())
            new_irnode = as<Block>()->clone();
        else if (is<Stmt>())
            new_irnode = as<Stmt>()->clone();
        else
            QUINT_NOT_IMPLEMENTED
        return new_irnode;
    }

    int StmtFieldSNode::get_snode_id(SNode *snode) {
        if (snode == nullptr)
            return -1;
        return snode->id;
    }

    bool StmtFieldSNode::equal(const StmtField *other_generic) const {
        if (auto other = dynamic_cast<const StmtFieldSNode *>(other_generic)) {
            return get_snode_id(snode_) == get_snode_id(other->snode_);
        }
        return false;
    }

    bool StmtFieldMemoryAccessOptions::equal(const StmtField *other_generic) const {
        if (auto other =
                dynamic_cast<const StmtFieldMemoryAccessOptions * >(other_generic)) {
            return opt_.get_all() == other->opt_.get_all();
        } else {
            return false;
        }
    }

    bool StmtFieldManager::equal(StmtFieldManager &other) const {
        return false;
    }

    std::atomic<int> Stmt::instance_id_counter(0);

    Stmt::Stmt(): field_manager(this), fields_registered(false) {
        parent = nullptr;
        instance_id = instance_id_counter++;
        id = instance_id;
        erased = false;
    }

    Stmt::Stmt(const Stmt &stmt): field_manager(this), fields_registered(false) {
        parent = stmt.parent;
        instance_id = instance_id_counter++;
        id = instance_id;
        erased = stmt.erased;
        tb = stmt.tb;
        ret_type = stmt.ret_type;
    }

    std::string Stmt::type_hint() const {
        if (ret_type->is_primitive(PrimitiveTypeID::unknown))
            return "";
        return fmt::format("<{}>", ret_type.to_string());
    }

    std::vector<Stmt *> Stmt::get_operands() const {
        std::vector<Stmt *> ret;
        for (int i = 0; i < num_operands(); i++) {
            ret.push_back(*operands[i]);
        }
        return ret;
    }

    void Stmt::set_operand(int i, Stmt *stmt) {
        *operands[i] = stmt;
    }

    void Stmt::register_operand(Stmt *&stmt) {
        operands.push_back(&stmt);
    }

    int Stmt::locate_operand(Stmt **stmt) {
        for (int i = 0; i < num_operands(); i++) {
            if (operands[i] == stmt) {
                return i;
            }
        }
        return -1;
    }

    void Stmt::mark_fields_registered() {
        QUINT_ASSERT(!fields_registered)
        fields_registered = true;
    }

    bool Stmt::has_operand(Stmt *stmt) const {
        for (int i = 0; i < num_operands(); i++) {
            if (*operands[i] == stmt) {
                return true;
            }
        }
        return false;
    }

    void Stmt::replace_usages_with(Stmt *new_stmt) {

    }

    void Stmt::replace_with(VecStatement &&new_statements, bool replace_usages) {
        parent->replace_with(this, std::move(new_statements), replace_usages);
    }

    void Stmt::replace_operand_with(Stmt *old_stmt, Stmt *new_stmt) {
        int n_op = num_operands();
        for (int i = 0; i < n_op; ++i) {
            if (operand(i) == old_stmt) {
                *operands[i] = new_stmt;
            }
        }
    }

    IRNode *Stmt::get_parent() const {
        return parent;
    }

    Stmt *Stmt::insert_before_me(std::unique_ptr<Stmt> &&new_stmt) {
        auto ret = new_stmt.get();
        QUINT_ASSERT(parent)
        auto iter = parent->find(this);
        QUINT_ASSERT(iter != parent->statements.end())
        parent->insert_at(std::move(new_stmt), iter);
        return ret;
    }

    Stmt *Stmt::insert_after_me(std::unique_ptr<Stmt> &&new_stmt) {
        auto ret = new_stmt.get();
        QUINT_ASSERT(parent)
        auto iter = parent->find(this);
        QUINT_ASSERT(iter != parent->statements.end())
        parent->insert_at(std::move(new_stmt), std::next(iter));
        return ret;
    }

    std::string Stmt::type() {
        return std::string();
    }


    Block *Block::parent_block() const {
        if (parent_stmt == nullptr)
            return nullptr;
        return parent_stmt->parent;
    }

    int Block::locate(Stmt *stmt) {
        for (int i = 0; i < (int) statements.size(); i++) {
            if (statements[i].get() == stmt)
                return i;
        }
        return -1;
    }

    stmt_vector::iterator Block::locate(int location) {
        if (location == -1)
            return statements.end();
        return statements.begin() + location;
    }

    stmt_vector::iterator Block::find(Stmt *stmt) {
        return std::find_if(statements.begin(), statements.end(),
                            [stmt](const pStmt &x) { return x.get() == stmt; });
    }

    void Block::erase(int location) {
        auto iter = locate(location);
        erase_range(iter, std::next(iter));
    }

    void Block::erase(Stmt *stmt) {
        auto iter = find(stmt);
        erase_range(iter, std::next(iter));
    }

    void Block::erase_range(stmt_vector::iterator begin, stmt_vector::iterator end) {
        for (auto iter = begin; iter != end; iter++) {
            (*iter)->erased = true;
            trash_bin.push_back(std::move(*iter));
        }
        statements.erase(begin, end);
    }

    void Block::erase(std::unordered_set<Stmt *> stmts) {
        stmt_vector clean_stmts;
        clean_stmts.reserve(statements.size());
        for (pStmt &stmt : statements) {
            if (stmts.find(stmt.get()) != stmts.end()) {
                stmt->erased = true;
                trash_bin.push_back(std::move(stmt));
            } else {
                clean_stmts.push_back(std::move(stmt));
            }
        }
        statements = std::move(clean_stmts);
    }

    std::unique_ptr<Stmt> Block::extract(int location) {
        auto stmt = std::move(statements[location]);
        statements.erase(statements.begin() + location);
        return stmt;
    }

    std::unique_ptr<Stmt> Block::extract(Stmt *stmt) {
        for (int i = 0; i < (int) statements.size(); i++) {
            if (statements[i].get() == stmt) {
                return extract(i);
            }
        }
        QUINT_ERROR("stmt not found")
    }

    Stmt *Block::insert(std::unique_ptr<Stmt> &&stmt, int location) {
        return insert_at(std::move(stmt), locate(location));
    }

    Stmt *Block::insert_at(std::unique_ptr<Stmt> &&stmt, stmt_vector::iterator location) {
        auto stmt_ptr = stmt.get();
        stmt->parent = this;
        statements.insert(location, std::move(stmt));
        return stmt_ptr;
    }

    Stmt *Block::insert(VecStatement &&stmt, int location) {
        return insert_at(std::move(stmt), locate(location));
    }

    Stmt *Block::insert_at(VecStatement &&stmt, stmt_vector::iterator location) {
        Stmt *stmt_ptr = nullptr;
        if (stmt.size()) {
            stmt_ptr = stmt.back().get();
        }
        for (auto &s : stmt.stmts) {
            s->parent = this;
        }
        statements.insert(location, std::make_move_iterator(stmt.stmts.begin()),
                          std::make_move_iterator(stmt.stmts.end()));
        return stmt_ptr;
    }

    void Block::replace_statements_in_range(int start, int end, VecStatement &&stmts) {
        QUINT_ASSERT(start <= end)
        erase_range(locate(start), locate(end));
        insert(std::move(stmts), start);
    }

    void Block::set_statements(VecStatement &&stmts) {
        statements.clear();
        for (int i = 0; i < (int) stmts.size(); i++) {
            insert(std::move(stmts[i]), i);
        }
    }

    void Block::replace_with(Stmt *old_statement, std::unique_ptr<Stmt> &&new_statement, bool replace_usages) {
        VecStatement vec;
        vec.push_back(std::move(new_statement));
        replace_with(old_statement, std::move(vec), replace_usages);
    }

    void Block::insert_before(Stmt *old_statement, VecStatement &&new_statements) {
        insert_at(std::move(new_statements), find(old_statement));
    }

    void Block::insert_after(Stmt *old_statement, VecStatement &&new_statements) {
        insert_at(std::move(new_statements), std::next(find(old_statement)));
    }

    void Block::replace_with(Stmt *old_statement, VecStatement &&new_statements, bool replace_usages) {
        auto iter = find(old_statement);
        QUINT_ASSERT(iter != statements.end())
        if (replace_usages && !new_statements.stmts.empty())
            old_statement->replace_usages_with(new_statements.back().get());
        trash_bin.push_back(std::move(*iter));
        if (new_statements.size() == 1) {
            // Keep all std::vector::iterator valid in this case.
            *iter = std::move(new_statements[0]);
            (*iter)->parent = this;
        } else {
            statements.erase(iter);
            insert_at(std::move(new_statements), iter);
        }
    }

    Stmt *Block::lookup_var(const Identifier &ident) const {
        auto ptr = local_var_to_stmt.find(ident);
        if (ptr != local_var_to_stmt.end()) {
            return ptr->second;
        } else {
            if (parent_block()) {
                return parent_block()->lookup_var(ident);
            } else {
                return nullptr;
            }
        }
    }

    IRNode *Block::get_parent() const {
        return parent_stmt;
    }

    std::unique_ptr<Block> Block::clone() const {
        auto new_block = std::make_unique<Block>();
        new_block->parent_stmt = parent_stmt;
        new_block->stop_gradients = stop_gradients;
        new_block->statements.reserve(size());
        for (auto &stmt : statements)
            new_block->insert(stmt->clone());
        return new_block;
    }

    DelayedIRModifier::~DelayedIRModifier() {
        QUINT_ASSERT(to_insert_before_.empty())
        QUINT_ASSERT(to_insert_after_.empty())
        QUINT_ASSERT(to_erase_.empty())
        QUINT_ASSERT(to_replace_with_.empty())
        QUINT_ASSERT(to_extract_to_block_front.empty())
        QUINT_ASSERT(to_type_check_.empty())
    }

    void DelayedIRModifier::erase(Stmt *stmt) {

    }

    void DelayedIRModifier::insert_before(Stmt *old_statement, std::unique_ptr<Stmt> new_statement) {

    }

    void DelayedIRModifier::insert_before(Stmt *old_statement, VecStatement &&new_statements) {

    }

    void DelayedIRModifier::insert_after(Stmt *old_statement, std::unique_ptr<Stmt> new_statement) {

    }

    void DelayedIRModifier::insert_after(Stmt *old_statement, VecStatement &&new_statements) {

    }

    void DelayedIRModifier::replace_with(Stmt *stmt, VecStatement &&new_statements, bool replace_usages) {

    }

    void DelayedIRModifier::extract_to_block_front(Stmt *stmt, Block *blk) {

    }

    void DelayedIRModifier::type_check(IRNode *node, CompileConfig cfg) {
        to_type_check_.emplace_back(node, cfg);
    }

    bool DelayedIRModifier::modify_ir() {
        bool force_modified = modified_;
        modified_ = false;
        if (to_insert_before_.empty() && to_insert_after_.empty() &&
            to_erase_.empty() && to_replace_with_.empty() &&
            to_extract_to_block_front.empty() && to_type_check_.empty())
            return force_modified;
        for (auto &i : to_insert_after_) {
            i.first->parent->insert_before(i.first, std::move(i.second));
        }
        to_insert_after_.clear();
        for (auto &stmt : to_erase_) {
            stmt->parent->erase(stmt);
        }
        to_erase_.clear();
        for (auto &i : to_replace_with_) {
            std::get<0>(i)->replace_with(std::move(std::get<1>(i)), std::get<2>(i));
        }
        to_insert_after_.clear();
        for (auto &stmt : to_erase_) {
            stmt->parent->erase(stmt);
        }
        to_erase_.clear();
        for (auto &i : to_replace_with_) {
            std::get<0>(i)->replace_with(std::move(std::get<1>(i)), std::get<2>(i));
        }
        to_replace_with_.clear();
        for (auto &i : to_extract_to_block_front) {
            auto extracted = i.first->parent->extract(i.first);
            i.second->insert(std::move(extracted), 0);
        }
        to_extract_to_block_front.clear();
        for (auto &i : to_type_check_) {
            irpass::type_check(i.first, i.second);
        }
        to_type_check_.clear();
        return true;
    }

    void DelayedIRModifier::mark_as_modified() {

    }

    ImmediateIRModifier::ImmediateIRModifier(IRNode *root) {
        stmt_usages_ = irpass::analysis::gather_statement_usages(root);
    }

    void ImmediateIRModifier::replace_usages_with(Stmt *old_stmt, Stmt *new_stmt) {

    }

}