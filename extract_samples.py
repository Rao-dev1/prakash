import json

transcript_path = r'C:\Users\Soham\.gemini\antigravity\brain\2f545a70-7a15-4434-9edb-7fe5dfa03617\.system_generated\logs\transcript_full.jsonl'

with open(transcript_path, 'r', encoding='utf-8') as f:
    for i, line in enumerate(f):
        if i == 61:
            data = json.loads(line)
            # Find in string representation or object values
            raw = str(data)
            
            target_hdr = 'component_id,lot_id,leakage_0h,leakage_24h,iddq_0h,iddq_24h'
            h_idx = raw.find(target_hdr + '\\nH0001')
            if h_idx == -1:
                h_idx = raw.find(target_hdr + '\nH0001')
            m_idx = raw.find(target_hdr + '\\nM0001')
            if m_idx == -1:
                m_idx = raw.find(target_hdr + '\nM0001')
            r_idx = raw.find(target_hdr + '\\nR0001')
            if r_idx == -1:
                r_idx = raw.find(target_hdr + '\nR0001')
                
            print('Found indices:', h_idx, m_idx, r_idx)
            
            if h_idx != -1 and m_idx != -1 and r_idx != -1:
                h_str = raw[h_idx:m_idx]
                m_str = raw[m_idx:r_idx]
                r_str = raw[r_idx:]
                # trim trailing json artifacts
                r_end = r_str.find("',")
                if r_end != -1:
                    r_str = r_str[:r_end]

                def format_csv(s):
                    return s.replace('\\n', '\n').replace('\\r', '').strip(" '\"")

                with open(r'C:\Users\Soham\.gemini\antigravity\scratch\prakash-ds\sample_data\healthy_components.csv', 'w', encoding='utf-8') as out:
                    out.write(format_csv(h_str) + '\n')
                with open(r'C:\Users\Soham\.gemini\antigravity\scratch\prakash-ds\sample_data\moderate_drift.csv', 'w', encoding='utf-8') as out:
                    out.write(format_csv(m_str) + '\n')
                with open(r'C:\Users\Soham\.gemini\antigravity\scratch\prakash-ds\sample_data\high_risk_reject.csv', 'w', encoding='utf-8') as out:
                    out.write(format_csv(r_str) + '\n')
                print('Saved sample files successfully!')
            break
