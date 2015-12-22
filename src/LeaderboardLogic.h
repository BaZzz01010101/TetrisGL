#pragma once
class LeaderboardLogic
{
public:
  enum State { stHidden, stShowing, stVisible, stHiding };
  enum Result { resNone, resClose };

  State state;
  float transitionProgress;
  int editRow;
  bool backButtonHighlighted;
  static const int leadersMaxCount = 10;
  static const int maxNameLength = 10;

  struct LeaderData
  {
    char name[maxNameLength + 1];
    int level;
    int score;
  };

  LeaderboardLogic();
  ~LeaderboardLogic();

  Result update();
  void escape();
  void load();
  void save();
  int getLeadersCount();
  const LeaderData & getLeaderData(int place);
  bool addResult(int level, int score);
  void deleteChar();
  void addChar(char ch);
  void commit();

private:
  std::vector<LeaderData> leaders;
};

