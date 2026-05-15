def chat(msg):
    responses = {
        "hi": "Hello! I am the Nexus AI Assistant.",
        "version": "You are running Nexus V4 ULTRA.",
        "speed": "Nexus is optimized for extreme performance."
    }
    return responses.get(msg.lower(), "Interesting. Tell me more about that.")
def analyze_sentiment(text):
    if "good" in text or "great" in text or "fast" in text:
        return "Positive"
    return "Neutral"
