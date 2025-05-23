#ifndef NODE_INCREMENT_OP_H
#define NODE_INCREMENT_OP_H

#include "../declaration/varTable.h"
#include "../node.h"
#include <memory>

namespace nicole {

class NodeIncrement final : public Node {
private:
  TokenType operator_;
  std::shared_ptr<Node> expression_;

public:
  NodeIncrement(const TokenType &op, std::shared_ptr<Node> expression,
                std::shared_ptr<Node> father = nullptr)
      : Node{NodeType::INCREMENT, father}, operator_{op},
        expression_{expression} {
  };

  virtual ~NodeIncrement() = default;

  const Node *expression() const { return expression_.get(); }

  TokenType op() const { return operator_; }

  llvm::Value *accept(const CodeGeneration *visitor) const override {
    return visitor->visit(this);
  }

  std::string accept(const PrintTree *visitor) const override {
    return visitor->visit(this);
  }
};

} // namespace nicole

#endif