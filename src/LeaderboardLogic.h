#pragma once
class LeaderboardLogic
{
public:
  typedef uint32_t Checksum;

  enum State { stHidden, stShowing, stVisible, stHiding };
  enum Result { resNone, resClose };

  State state;
  const float showingTime;
  const float hidingTime;
  float transitionProgress;
  int editRow;
  bool backButtonHighlighted;
  static const int leadersCount = 10;
  static const int maxNameLength = 15;

  struct LeaderData
  {
    char name[maxNameLength + 1];
    int level;
    int score;
  };

  LeaderboardLogic();
  ~LeaderboardLogic();

  void init();
  Result update();
  void escape();
  bool load();
  void save();
  const LeaderData & getLeaderData(int place);
  bool addResult(int level, int score);
  void deleteChar();
  void addChar(char ch);
  void commit();

private:
  std::string fileName;
  static const LeaderData defaultLeaders[leadersCount];
  LeaderData leaders[leadersCount];
};

