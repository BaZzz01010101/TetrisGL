#include "static_headers.h"

#include "LeaderboardLogic.h"
#include "Globals.h"
#include "Time.h"
#include "Crosy.h"

LeaderboardLogic::LeaderboardLogic() :
  state(stHidden),
  transitionProgress(0.0f),
  editRow(-1),
  backButtonHighlighted(false)
{
  load();
  //leaders.push_back({ "EVERY", 15, 99999999 });
  //leaders.push_back({ "HUNTER", 14, 9999999 });
  //leaders.push_back({ "WANT", 13, 999999 });
  //leaders.push_back({ "TO", 12, 99999 });
  //leaders.push_back({ "KNOW", 11, 9999 });
  //leaders.push_back({ "WHERE", 10, 999 });
  //leaders.push_back({ "PHEASANT", 9, 99 });
  //leaders.push_back({ "SITS", 8, 9 });
  //leaders.push_back({ "BAZZZ", 8, 9 });
  //leaders.push_back({ "ZEUS", 8, 9 });
}


LeaderboardLogic::~LeaderboardLogic()
{
}

LeaderboardLogic::Result LeaderboardLogic::update()
{
  Result retResult = resNone;

  switch (state)
  {
  case stHidden:
    state = stShowing;
    break;

  case stShowing:
    if ((transitionProgress += Time::timerDelta / Globals::leaderboardShowingTime) >= 1.0f)
    {
      transitionProgress = 1.0f;
      state = stVisible;
    }

    break;

  case stVisible:
    break;

  case stHiding:
    if ((transitionProgress -= Time::timerDelta / Globals::leaderboardHidingTime) <= 0.0f)
    {
      transitionProgress = 0.0f;
      state = stHidden;
      retResult = resClose;
    }

    break;

  default:
    assert(0);
    break;
  }

  return retResult;
}

void LeaderboardLogic::escape()
{
  assert(state == stVisible);

  state = stHiding;
}

void LeaderboardLogic::load()
{
  std::string fileName = Crosy::getExePath() + "leaderboard.dat";
  FILE * file = fopen(fileName.c_str(), "rb+");
  fseek(file, 0, SEEK_END);
  const int size = ftell(file);
  fseek(file, 0, SEEK_SET);
  const int chunkSize = sizeof(uint32_t);

  if (size % chunkSize == 0)
  {
    std::vector<uint32_t> buf(size / chunkSize);
    fread(buf.data(), 1, size, file);
    uint32_t checksum = 0;

    for (int i = 0; i < size / chunkSize; i++)
      checksum = checksum ^ buf[i];

    if (!checksum)
    {
      if ((size - chunkSize) % sizeof(LeaderData) == 0)
      {
        const int fileLeadersCount = (size - chunkSize) / sizeof(LeaderData);
        leaders.resize(glm::min(fileLeadersCount, leadersMaxCount));
        memcpy(leaders.data(), buf.data(), leaders.size() * sizeof(LeaderData));
      }
    }
  }

  fclose(file);
}

void LeaderboardLogic::save()
{
  std::string fileName = Crosy::getExePath() + "leaderboard.dat";
  FILE * file = fopen(fileName.c_str(), "wb+");
  const int leadersSize = getLeadersCount() * sizeof(LeaderData);
  const int chunkSize = sizeof(uint32_t);
  const int size = leadersSize / chunkSize * chunkSize + ((leadersSize % chunkSize > 0) ? 2 * chunkSize : chunkSize);
  std::vector<uint32_t> buf(size / chunkSize);
  memset(buf.data(), 0, size);
  memcpy(buf.data(), leaders.data(), leaders.size() * sizeof(LeaderData));

  uint32_t checksum = 0;

  for (int i = 0; i < size / chunkSize; i++)
    checksum = checksum ^ buf[i];

  buf[size / chunkSize - 1] = checksum;
  fwrite(buf.data(), 1, size, file);
  fclose(file);
}

int LeaderboardLogic::getLeadersCount()
{
  return leaders.size();
}

const LeaderboardLogic::LeaderData & LeaderboardLogic::getLeaderData(int place)
{
  if (place >= (int)leaders.size())
  {
    assert(0);
    static LeaderData dummy;
    return dummy;
  }

  return leaders[place];
}

bool LeaderboardLogic::addResult(int level, int score)
{
  if (leaders.size() < leadersMaxCount)
    leaders.resize(leaders.size() + 1);

  for (int i = 0, cnt = leaders.size(); i < cnt; i++)
  {
    LeaderData & leader = leaders[i];

    if (score > leader.score || (score == leader.score && level > leader.level))
    {
      const LeaderData newLeader = { { '\0' }, level, score };
      leaders.insert(leaders.begin() + i, newLeader);

      if (leaders.size() > leadersMaxCount)
        leaders.resize(leadersMaxCount);

      editRow = i;
      return true;
    }
  }

  return false;
}

void LeaderboardLogic::deleteChar()
{
  assert(editRow >= 0);

  if (editRow >= 0)
  {
    char * name = leaders[editRow].name;
    int nameLen = strlen(name);

    if (nameLen)
      name[nameLen - 1] = '\0';
  }
}

void LeaderboardLogic::addChar(char ch)
{
  assert(editRow >= 0);

  if (editRow >= 0)
  {
    char * name = leaders[editRow].name;
    int nameLen = strlen(name);

    if (nameLen < maxNameLength)
    {
      name[nameLen] = ch;
      name[nameLen + 1] = '\0';
    }
  }
}

void LeaderboardLogic::commit()
{
  assert(editRow >= 0);

  if (editRow >= 0)
  {
    editRow = -1;
    save();
    escape();
  }
}
