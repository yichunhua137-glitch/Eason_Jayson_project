export module vertex;

import residence;

export class Vertex{
  private:
    int id;
    Residence *residence;

  public:
    Vertex(int id);
    int getId() const;
    bool hasResidence() const;
    Residence *getResidence() const;
    void buildResidence(Residence *residence);
};
