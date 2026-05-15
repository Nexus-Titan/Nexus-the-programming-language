pub fn encrypt_vault(data: &str, key: u32) -> String {
    data.chars()
        .map(|c| ((c as u32) ^ key) as u8 as char)
        .collect()
}
pub fn schedule_task(id: u32, delay_ms: u64) {
    println!("Task {} scheduled for {}ms", id, delay_ms);
}
