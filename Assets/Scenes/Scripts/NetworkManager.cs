using UnityEngine;
using UnityEngine.UI;
using System.Net.Sockets;
using System.IO;
using System;
using System.Threading;

public class NetworkManager : MonoBehaviour
{
    [Header("Connection")]
    public string serverIp = "127.0.0.1";
    public int serverPort = 12345;

    [Header("UI Assignments")]
    public Slider myHpBar;
    public Slider opHpBar;
    public Text statusText;  // Center text log
    public Button[] skillButtons; // Array for our 3 buttons

    private TcpClient client;
    private StreamReader reader;
    private StreamWriter writer;
    private Thread networkThread;

    // Variables to pass data safely to Unity's main thread
    private string pendingJson = null;
    private string pendingLog = null;

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

            statusText.text = "Connected to Server!";
            
            // Start listening in the background
            networkThread = new Thread(ListenForMessages);
            networkThread.IsBackground = true;
            networkThread.Start();
        }
        catch (Exception e)
        {
            statusText.text = "Error: " + e.Message;
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
                {
                    pendingJson = line; // It's game data!
                }
                else if (!line.Contains("Select Skill")) 
                {
                    pendingLog = line; // It's a battle log!
                }
            }
            catch { break; }
        }
    }

    void Update()
    {
        // 1. Update Visuals if we got JSON
        if (pendingJson != null)
        {
            UpdateVisuals(pendingJson);
            pendingJson = null;
        }

        // 2. Update Text if we got a Log
        if (pendingLog != null)
        {
            statusText.text = pendingLog;
            pendingLog = null;
        }
    }

    void UpdateVisuals(string json)
    {
        GameState state = JsonUtility.FromJson<GameState>(json);

        // Update Sliders
        myHpBar.maxValue = state.myMaxHp;
        myHpBar.value = state.myHp;
        opHpBar.maxValue = state.opMaxHp;
        opHpBar.value = state.opHp;

        // Update Buttons
        for (int i = 0; i < skillButtons.Length; i++)
        {
            if (i < state.skills.Count)
            {
                Text btnText = skillButtons[i].GetComponentInChildren<Text>();
                btnText.text = $"{state.skills[i].name} ({state.skills[i].damage})";
                skillButtons[i].interactable = state.isMyTurn;
            }
        }
        
        if(state.isMyTurn) statusText.text = ">>> YOUR TURN <<<";
    }

    // This function will be called by our UI Buttons
    public void SendAttack(int skillIndex)
    {
        if (client == null) return;
        try 
        { 
            writer.WriteLine(skillIndex.ToString());
            // Disable buttons immediately so we don't click twice
            foreach(var btn in skillButtons) btn.interactable = false;
        }
        catch (Exception e) { statusText.text = "Error: " + e.Message; }
    }

    void OnApplicationQuit()
    {
        if (client != null) client.Close();
        if (networkThread != null) networkThread.Abort();
    }
}