using UnityEngine;
using UnityEngine.UI;
using System.Net.Sockets;
using System.IO;
using System;
using System.Threading;
using System.Collections.Generic;

public class NetworkManager : MonoBehaviour
{
    [Header("Connection")]
    public string serverIp = "127.0.0.1";
    public int serverPort = 12345;

    [Header("UI Assignments")]
    public Slider myHpBar;
    public Text myHpText;
    public Slider opHpBar;
    public Text opHpText;
    public Text statusText;
    public Button[] skillButtons;

    [Header("Animation Settings")]
    public float hpFillSpeed = 5f;

    private float myHpTarget;
    private float opHpTarget;

    private int lastMyHp = -1;
    private int lastOpHp = -1;

    private TcpClient client;
    private StreamReader reader;
    private StreamWriter writer;
    private Thread networkThread;

    private string pendingJson;
    private Queue<string> pendingLogs = new Queue<string>();
    private List<string> logHistory = new List<string>();
    public int maxLogLines = 8;

    private bool isMyTurnCached = false;
    private bool lastTurnState = false;

    [Header("Characters")]
    public CharacterAnimator myCharacter;
    public CharacterAnimator enemyCharacter;

    void Start()
    {
        ConnectToCServer();
    }

    void ConnectToCServer()
    {
        try
        {
            client = new TcpClient(serverIp, serverPort);
            NetworkStream stream = client.GetStream();
            reader = new StreamReader(stream);
            writer = new StreamWriter(stream) { AutoFlush = true };

            LogToHistory("Connected to server.");

            networkThread = new Thread(ListenForMessages);
            networkThread.IsBackground = true;
            networkThread.Start();
        }
        catch (Exception e)
        {
            LogToHistory("Connection error: " + e.Message);
        }
    }

    void ListenForMessages()
    {
        while (client != null && client.Connected)
        {
            try
            {
                string line = reader.ReadLine();
                if (line == null) break;

                if (line.StartsWith("{"))
                    pendingJson = line;
                else
                    pendingLogs.Enqueue(line);
            }
            catch { break; }
        }
    }

    void Update()
    {
        if (pendingJson != null)
        {
            UpdateVisuals(pendingJson);
            pendingJson = null;
        }

        while (pendingLogs.Count > 0)
        {
            string log = pendingLogs.Dequeue();
            if (log.Contains("Select Skill")) continue;
            LogToHistory(log.Trim());
        }

        SmoothUpdateHP();
    }

    void UpdateVisuals(string json)
    {
        GameState state = JsonUtility.FromJson<GameState>(json);

        myHpBar.maxValue = state.myMaxHp;
        opHpBar.maxValue = state.opMaxHp;

        // First sync
        if (lastMyHp < 0)
        {
            myHpBar.value = state.myHp;
            opHpBar.value = state.opHp;

            myHpTarget = state.myHp;
            opHpTarget = state.opHp;

            lastMyHp = state.myHp;
            lastOpHp = state.opHp;
            return;
        }

        bool myHpDecreased = state.myHp < lastMyHp;
        bool opHpDecreased = state.opHp < lastOpHp;

        // 🔥 AUTHORITATIVE DAMAGE LOGIC
        if (myHpDecreased && !opHpDecreased)
        {
            enemyCharacter.PlayAttack(Vector3.left);
            myCharacter.PlayHit();
            LogToHistory("Opponent attacks you!");
        }
        else if (opHpDecreased && !myHpDecreased)
        {
            myCharacter.PlayAttack(Vector3.right);
            enemyCharacter.PlayHit();
            LogToHistory("You attack the opponent!");
        }

        myHpTarget = state.myHp;
        opHpTarget = state.opHp;

        lastMyHp = state.myHp;
        lastOpHp = state.opHp;

        isMyTurnCached = state.isMyTurn;

        // Turn text (no duplicates)
        if (isMyTurnCached != lastTurnState)
        {
            LogToHistory(isMyTurnCached
                ? ">>> YOUR TURN <<<"
                : "Enemy is thinking...");
        }
        lastTurnState = isMyTurnCached;

        // Buttons
        for (int i = 0; i < skillButtons.Length; i++)
        {
            if (i < state.skills.Count)
            {
                Text btnText = skillButtons[i].GetComponentInChildren<Text>();
                btnText.text = $"{state.skills[i].name} ({state.skills[i].damage})";
                skillButtons[i].interactable = isMyTurnCached;
            }
        }
    }

    void SmoothUpdateHP()
    {
        myHpBar.value = Mathf.MoveTowards(
            myHpBar.value,
            myHpTarget,
            hpFillSpeed * Time.deltaTime * 100f
        );

        opHpBar.value = Mathf.MoveTowards(
            opHpBar.value,
            opHpTarget,
            hpFillSpeed * Time.deltaTime * 100f
        );

        myHpText.text = $"{Mathf.RoundToInt(myHpTarget)} / {myHpBar.maxValue}";
        opHpText.text = $"{Mathf.RoundToInt(opHpTarget)} / {opHpBar.maxValue}";
    }

    public void SendAttack(int skillIndex)
    {
        if (client == null || !isMyTurnCached) return;

        try
        {
            writer.WriteLine(skillIndex.ToString());
            foreach (var btn in skillButtons)
                btn.interactable = false;
        }
        catch (Exception e)
        {
            LogToHistory("Send error: " + e.Message);
        }
    }

    void LogToHistory(string message)
    {
        if (logHistory.Count >= maxLogLines)
            logHistory.RemoveAt(0);

        logHistory.Add(message);
        statusText.text = string.Join("\n", logHistory);
    }

    void OnApplicationQuit()
    {
        if (client != null) client.Close();
        if (networkThread != null) networkThread.Abort();
    }
}
