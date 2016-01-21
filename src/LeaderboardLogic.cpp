#include "static_headers.h"

#include "LeaderboardLogic.h"
#include "Globals.h"
#include "Time.h"
#include "Crosy.h"

const LeaderboardLogic::LeaderData LeaderboardLogic::defaultLeaders[LeaderboardLogic::leadersCount] =
{
  { "ARNOLD", 13, 12000 },
  { "SARA", 11, 9000 },
  { "JOHN", 9, 6000 },
  { "NIKOL", 7, 4000 },
  { "SAM", 6, 3000 },
  { "LARA", 5, 2000 },
  { "BRUCE", 4, 1500 },
  { "CINDY", 3, 1000 },
  { "BOB", 2, 500 },
  { "HELEN", 1, 300 },
};

LeaderboardLogic::LeaderboardLogic() :
  state(stHidden),
  showingTime(0.3f),
  hidingTime(0.2f),
  transitionProgress(0.0f),
  editRow(-1),
  backButtonHighlighted(false)
{
}


LeaderboardLogic::~LeaderboardLogic()
{
}

void LeaderboardLogic::init()
{
  fileName = Crosy::getExePath() + "leaderboard.dat";

  if (!load())
    memcpy(leaders, defaultLeaders, sizeof(leaders));
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
    if ((transitionProgress += Time::timerDelta / showingTime) >= 1.0f)
    {
      transitionProgress = 1.0f;
      state = stVisible;
    }

    break;

  case stVisible:
    break;

  case stHiding:
    if ((transitionProgress -= Time::timerDelta / hidingTime) <= 0.0f)
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

bool LeaderboardLogic::load()
{
  bool success = false;
  FILE * file = fopen(fileName.c_str(), "rb+");

  if (file)
  {
    fseek(file, 0, SEEK_END);
    const int fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    Checksum checksum = 0;
    const int checksumSize = sizeof(checksum);
    const int leadersSize = leadersCount * sizeof(LeaderData);
    const int checksumElementsCount = (leadersSize - 1) / checksumSize + 1;
    Checksum buf[checksumElementsCount + 1];
    memset(buf, 0, sizeof(buf));

    if (fileSize == sizeof(buf) && fread(buf, sizeof(buf), 1, file))
    {
      for (int i = 0; i < checksumElementsCount; i++)
        checksum ^= buf[i] * i;

      if (checksum == buf[checksumElementsCount])
      {
        memcpy(leaders, buf, sizeof(leaders));
        success = true;
      }
    }

    fclose(file);
  }

  return success;
}

void LeaderboardLogic::save()
{
  assert(leadersCount > 0);

  FILE * file = fopen(fileName.c_str(), "wb+");
  assert(file);

  if (file)
  {
    Checksum checksum = 0;
    const int checksumSize = sizeof(checksum);
    const int leadersSize = leadersCount * sizeof(LeaderData);
    const int checksumElementsCount = (leadersSize - 1) / checksumSize + 1;
    Checksum buf[checksumElementsCount + 1];
    memset(buf, 0, sizeof(buf));
    memcpy(buf, leaders, sizeof(leaders));

    for (int i = 0; i < checksumElementsCount; i++)
      checksum ^= buf[i] * i;

    buf[checksumElementsCount] = checksum;
    int success = (int)fwrite(buf, sizeof(buf), 1, file);
    fclose(file);
    assert(success);
  }
}

const LeaderboardLogic::LeaderData & LeaderboardLogic::getLeaderData(int place)
{
  if (place >= leadersCount)
  {
    assert(0);
    static LeaderData dummy = { { '\0' }, 0, 0 };
    return dummy;
  }

  return leaders[place];
}

bool LeaderboardLogic::addResult(int level, int score)
{
  for (int i = 0; i < leadersCount; i++)
  {
    LeaderData & leader = leaders[i];

    if (score > leader.score || (score == leader.score && level > leader.level))
    {
      if (i < leadersCount - 1)
        memmove(leaders + i + 1, leaders + i, (leadersCount - i - 1) * sizeof(LeaderData));

      leader.name[0] = '\0';
      leader.level = level;
      leader.score = score;
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
    int nameLen = (int)strlen(name);

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
    int nameLen = (int)strlen(name);

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
