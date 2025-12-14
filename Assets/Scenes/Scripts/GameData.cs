using System;
using System.Collections.Generic;

// These classes match the JSON sent by your C Server
[Serializable]
public class SkillData
{
    public int index;
    public string name;
    public int damage;
}

[Serializable]
public class GameState
{
    public int myHp;
    public int myMaxHp;
    public string myElement;
    
    public int opHp;
    public int opMaxHp;
    public string opElement;
    
    public bool isMyTurn;
    public List<SkillData> skills;
}