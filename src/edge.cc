export module edge;

import types;

export class Edge{
  private:
    int id;
    bool built;
    Colour owner;

  public:
    Edge(int id);
    int getId() const;
    bool isBuilt() const;
    Colour getOwner() const;
    void build(Colour owner);
};