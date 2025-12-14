using UnityEngine;
using System.Collections;

public class CharacterAnimator : MonoBehaviour
{
    [Header("Breathing")]
    public float breatheAmplitude = 0.03f;
    public float breatheSpeed = 2f;

    [Header("Attack")]
    public float attackDistance = 0.25f;
    public float attackDuration = 0.12f;
    public float shakeIntensity = 0.08f;

    private Vector3 startPos;
    private Vector3 startScale;
    private bool isAttacking = false;

    void Start()
    {
        startPos = transform.localPosition;
        startScale = transform.localScale;
    }

    void Update()
    {
        if (!isAttacking)
        {
            // Smooth breathing (scale-based = pixel safe)
            float breathe = Mathf.Sin(Time.time * breatheSpeed) * breatheAmplitude;
            transform.localScale = startScale + Vector3.one * breathe;
        }
    }

    // ---------- ATTACK ----------
    public void PlayAttack(Vector3 direction)
    {
        if (!isAttacking)
            StartCoroutine(AttackRoutine(direction));
    }

    // ---------- HIT / SHAKE ----------
    public void PlayHit()
    {
        if (!isAttacking)
            StartCoroutine(ShakeRoutine());
    }

    IEnumerator AttackRoutine(Vector3 dir)
    {
        isAttacking = true;

        Vector3 target = startPos + dir * attackDistance;

        float t = 0;
        while (t < 1)
        {
            t += Time.deltaTime / attackDuration;
            transform.localPosition = Vector3.Lerp(startPos, target, t);
            yield return null;
        }

        t = 0;
        while (t < 1)
        {
            t += Time.deltaTime / attackDuration;
            transform.localPosition = Vector3.Lerp(target, startPos, t);
            yield return null;
        }

        transform.localPosition = startPos;
        isAttacking = false;
    }

    IEnumerator ShakeRoutine()
    {
        isAttacking = true;

        float timer = 0.15f;
        while (timer > 0)
        {
            timer -= Time.deltaTime;
            transform.localPosition = startPos +
                (Vector3)Random.insideUnitCircle * shakeIntensity;
            yield return null;
        }

        transform.localPosition = startPos;
        isAttacking = false;
    }
}
