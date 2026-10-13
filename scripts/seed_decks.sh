#!/bin/sh
# Insert 12 sample decks with 40 cards each. Existing decks are left in place.
set -eu

root=$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)
db=${1:-"$root/merken.db"}

if ! command -v sqlite3 >/dev/null 2>&1; then
	echo "sqlite3 is required" >&2
	exit 1
fi

if [ ! -f "$db" ]; then
	echo "Database not found: $db" >&2
	echo "Open Merken once so it can create the database, then run this again." >&2
	exit 1
fi

sqlite3 "$db" <<'SQL'
PRAGMA foreign_keys = ON;
BEGIN IMMEDIATE;
INSERT INTO decks (name, description, created_at) VALUES ('Spanish for the Road', 'Phrases for airports, cafés, streets, and hotels. Say the Spanish out loud before you flip the card.', date('now'));
INSERT INTO cards (deck_id, front, back, tags, card_type, choices, interval_days, ease_factor, repetition_count, next_review_date, last_reviewed_date)
SELECT deck.deck_id, card.front, card.back, card.tags, 0, '', 0, 2.5, 0, '', ''
FROM (SELECT last_insert_rowid() AS deck_id) AS deck
JOIN (
SELECT 'Hello' AS front, 'Hola' AS back, 'greeting, essential' AS tags
UNION ALL
SELECT 'Good morning' AS front, 'Buenos días' AS back, 'greeting, time' AS tags
UNION ALL
SELECT 'Good afternoon' AS front, 'Buenas tardes' AS back, 'greeting, time' AS tags
UNION ALL
SELECT 'Good evening / good night' AS front, 'Buenas noches' AS back, 'greeting, time' AS tags
UNION ALL
SELECT 'Please' AS front, 'Por favor' AS back, 'polite, essential' AS tags
UNION ALL
SELECT 'Thank you' AS front, 'Gracias' AS back, 'polite, essential' AS tags
UNION ALL
SELECT 'You''re welcome' AS front, 'De nada' AS back, 'polite, essential' AS tags
UNION ALL
SELECT 'Excuse me / sorry' AS front, 'Perdón, or disculpe when addressing someone' AS back, 'polite, essential' AS tags
UNION ALL
SELECT 'Yes' AS front, 'Sí' AS back, 'essential' AS tags
UNION ALL
SELECT 'No' AS front, 'No' AS back, 'essential' AS tags
UNION ALL
SELECT 'Where is the bathroom?' AS front, '¿Dónde está el baño?' AS back, 'travel, question' AS tags
UNION ALL
SELECT 'How much does it cost?' AS front, '¿Cuánto cuesta?' AS back, 'shopping, question' AS tags
UNION ALL
SELECT 'I would like…' AS front, 'Quisiera…' AS back, 'café, polite' AS tags
UNION ALL
SELECT 'The check, please' AS front, 'La cuenta, por favor' AS back, 'café, polite' AS tags
UNION ALL
SELECT 'I don''t understand' AS front, 'No entiendo' AS back, 'conversation, essential' AS tags
UNION ALL
SELECT 'Do you speak English?' AS front, '¿Habla inglés?' AS back, 'conversation, question' AS tags
UNION ALL
SELECT 'Can you help me?' AS front, '¿Me puede ayudar?' AS back, 'travel, question' AS tags
UNION ALL
SELECT 'A table for two' AS front, 'Una mesa para dos' AS back, 'café' AS tags
UNION ALL
SELECT 'Water, please' AS front, 'Agua, por favor' AS back, 'café, essential' AS tags
UNION ALL
SELECT 'Without ice' AS front, 'Sin hielo' AS back, 'café' AS tags
UNION ALL
SELECT 'I am allergic to…' AS front, 'Soy alérgico a… / Soy alérgica a…' AS back, 'café, health' AS tags
UNION ALL
SELECT 'Left' AS front, 'Izquierda' AS back, 'directions' AS tags
UNION ALL
SELECT 'Right' AS front, 'Derecha' AS back, 'directions' AS tags
UNION ALL
SELECT 'Straight ahead' AS front, 'Todo recto' AS back, 'directions' AS tags
UNION ALL
SELECT 'Train station' AS front, 'La estación de tren' AS back, 'travel, place' AS tags
UNION ALL
SELECT 'Airport' AS front, 'El aeropuerto' AS back, 'travel, place' AS tags
UNION ALL
SELECT 'Ticket' AS front, 'El boleto, or el billete in Spain' AS back, 'travel' AS tags
UNION ALL
SELECT 'What time is it?' AS front, '¿Qué hora es?' AS back, 'time, question' AS tags
UNION ALL
SELECT 'Today' AS front, 'Hoy' AS back, 'time' AS tags
UNION ALL
SELECT 'Tomorrow' AS front, 'Mañana' AS back, 'time' AS tags
UNION ALL
SELECT 'Yesterday' AS front, 'Ayer' AS back, 'time' AS tags
UNION ALL
SELECT 'Open' AS front, 'Abierto' AS back, 'sign' AS tags
UNION ALL
SELECT 'Closed' AS front, 'Cerrado' AS back, 'sign' AS tags
UNION ALL
SELECT 'I need a doctor' AS front, 'Necesito un médico' AS back, 'health, essential' AS tags
UNION ALL
SELECT 'Call the police' AS front, 'Llame a la policía' AS back, 'emergency' AS tags
UNION ALL
SELECT 'My name is…' AS front, 'Me llamo…' AS back, 'introduction' AS tags
UNION ALL
SELECT 'Nice to meet you' AS front, 'Mucho gusto' AS back, 'introduction' AS tags
UNION ALL
SELECT 'See you later' AS front, 'Hasta luego' AS back, 'greeting' AS tags
UNION ALL
SELECT 'How are you?' AS front, '¿Cómo está?' AS back, 'greeting, question' AS tags
UNION ALL
SELECT 'I am lost' AS front, 'Estoy perdido / Estoy perdida' AS back, 'travel, essential' AS tags
) AS card;
INSERT INTO decks (name, description, created_at) VALUES ('French Café and Street', 'French you can use the same day: ordering, asking the way, and being polite.', date('now'));
INSERT INTO cards (deck_id, front, back, tags, card_type, choices, interval_days, ease_factor, repetition_count, next_review_date, last_reviewed_date)
SELECT deck.deck_id, card.front, card.back, card.tags, 0, '', 0, 2.5, 0, '', ''
FROM (SELECT last_insert_rowid() AS deck_id) AS deck
JOIN (
SELECT 'Hello' AS front, 'Bonjour' AS back, 'greeting, essential' AS tags
UNION ALL
SELECT 'Good evening' AS front, 'Bonsoir' AS back, 'greeting, time' AS tags
UNION ALL
SELECT 'Goodbye' AS front, 'Au revoir' AS back, 'greeting' AS tags
UNION ALL
SELECT 'Please' AS front, 'S''il vous plaît' AS back, 'polite, essential' AS tags
UNION ALL
SELECT 'Thank you' AS front, 'Merci' AS back, 'polite, essential' AS tags
UNION ALL
SELECT 'You''re welcome' AS front, 'Je vous en prie, or de rien' AS back, 'polite' AS tags
UNION ALL
SELECT 'Excuse me' AS front, 'Excusez-moi' AS back, 'polite' AS tags
UNION ALL
SELECT 'Yes' AS front, 'Oui' AS back, 'essential' AS tags
UNION ALL
SELECT 'No' AS front, 'Non' AS back, 'essential' AS tags
UNION ALL
SELECT 'I would like a coffee' AS front, 'Je voudrais un café' AS back, 'café' AS tags
UNION ALL
SELECT 'A coffee with milk' AS front, 'Un café au lait' AS back, 'café' AS tags
UNION ALL
SELECT 'The bill, please' AS front, 'L''addition, s''il vous plaît' AS back, 'café' AS tags
UNION ALL
SELECT 'Water, still' AS front, 'De l''eau plate' AS back, 'café' AS tags
UNION ALL
SELECT 'Sparkling water' AS front, 'De l''eau pétillante' AS back, 'café' AS tags
UNION ALL
SELECT 'I am vegetarian' AS front, 'Je suis végétarien / végétarienne' AS back, 'café, diet' AS tags
UNION ALL
SELECT 'Without sugar' AS front, 'Sans sucre' AS back, 'café' AS tags
UNION ALL
SELECT 'Where is the metro?' AS front, 'Où est le métro ?' AS back, 'directions, question' AS tags
UNION ALL
SELECT 'Left' AS front, 'À gauche' AS back, 'directions' AS tags
UNION ALL
SELECT 'Right' AS front, 'À droite' AS back, 'directions' AS tags
UNION ALL
SELECT 'Straight ahead' AS front, 'Tout droit' AS back, 'directions' AS tags
UNION ALL
SELECT 'I am lost' AS front, 'Je suis perdu / perdue' AS back, 'travel' AS tags
UNION ALL
SELECT 'Do you speak English?' AS front, 'Parlez-vous anglais ?' AS back, 'conversation' AS tags
UNION ALL
SELECT 'I don''t understand' AS front, 'Je ne comprends pas' AS back, 'conversation' AS tags
UNION ALL
SELECT 'Can you repeat that?' AS front, 'Pouvez-vous répéter ?' AS back, 'conversation' AS tags
UNION ALL
SELECT 'More slowly, please' AS front, 'Plus lentement, s''il vous plaît' AS back, 'conversation' AS tags
UNION ALL
SELECT 'How much is it?' AS front, 'C''est combien ?' AS back, 'shopping' AS tags
UNION ALL
SELECT 'Too expensive' AS front, 'C''est trop cher' AS back, 'shopping' AS tags
UNION ALL
SELECT 'I would like this one' AS front, 'Je voudrais celui-ci / celle-ci' AS back, 'shopping' AS tags
UNION ALL
SELECT 'Open' AS front, 'Ouvert' AS back, 'sign' AS tags
UNION ALL
SELECT 'Closed' AS front, 'Fermé' AS back, 'sign' AS tags
UNION ALL
SELECT 'Today' AS front, 'Aujourd''hui' AS back, 'time' AS tags
UNION ALL
SELECT 'Tomorrow' AS front, 'Demain' AS back, 'time' AS tags
UNION ALL
SELECT 'What time is it?' AS front, 'Quelle heure est-il ?' AS back, 'time' AS tags
UNION ALL
SELECT 'Train station' AS front, 'La gare' AS back, 'place' AS tags
UNION ALL
SELECT 'Airport' AS front, 'L''aéroport' AS back, 'place' AS tags
UNION ALL
SELECT 'A ticket to…' AS front, 'Un billet pour…' AS back, 'travel' AS tags
UNION ALL
SELECT 'My name is…' AS front, 'Je m''appelle…' AS back, 'introduction' AS tags
UNION ALL
SELECT 'Nice to meet you' AS front, 'Enchanté / Enchantée' AS back, 'introduction' AS tags
UNION ALL
SELECT 'See you soon' AS front, 'À bientôt' AS back, 'greeting' AS tags
UNION ALL
SELECT 'Help!' AS front, 'Au secours !' AS back, 'emergency' AS tags
) AS card;
INSERT INTO decks (name, description, created_at) VALUES ('Japanese for Daily Life', 'Forty core words and phrases, with romaji and a short note on when to use them.', date('now'));
INSERT INTO cards (deck_id, front, back, tags, card_type, choices, interval_days, ease_factor, repetition_count, next_review_date, last_reviewed_date)
SELECT deck.deck_id, card.front, card.back, card.tags, 0, '', 0, 2.5, 0, '', ''
FROM (SELECT last_insert_rowid() AS deck_id) AS deck
JOIN (
SELECT 'Hello' AS front, 'Konnichiwa. Use ohayō gozaimasu in the morning.' AS back, 'greeting' AS tags
UNION ALL
SELECT 'Thank you' AS front, 'Arigatō gozaimasu. Arigatō is fine with friends.' AS back, 'polite' AS tags
UNION ALL
SELECT 'Thanks for the meal, before eating' AS front, 'Itadakimasu' AS back, 'food, polite' AS tags
UNION ALL
SELECT 'Thanks for the meal, after eating' AS front, 'Gochisōsama deshita' AS back, 'food, polite' AS tags
UNION ALL
SELECT 'Excuse me / sorry' AS front, 'Sumimasen' AS back, 'polite, essential' AS tags
UNION ALL
SELECT 'Yes' AS front, 'Hai' AS back, 'essential' AS tags
UNION ALL
SELECT 'No' AS front, 'Iie' AS back, 'essential' AS tags
UNION ALL
SELECT 'Please give me this' AS front, 'Kore o kudasai' AS back, 'shopping' AS tags
UNION ALL
SELECT 'How much is it?' AS front, 'Ikura desu ka?' AS back, 'shopping, question' AS tags
UNION ALL
SELECT 'Where is the station?' AS front, 'Eki wa doko desu ka?' AS back, 'travel, question' AS tags
UNION ALL
SELECT 'Train station' AS front, 'Eki' AS back, 'place' AS tags
UNION ALL
SELECT 'This one' AS front, 'Kore' AS back, 'pointer' AS tags
UNION ALL
SELECT 'That one, near you' AS front, 'Sore' AS back, 'pointer' AS tags
UNION ALL
SELECT 'That one, over there' AS front, 'Are' AS back, 'pointer' AS tags
UNION ALL
SELECT 'Water' AS front, 'Mizu' AS back, 'food' AS tags
UNION ALL
SELECT 'Tea' AS front, 'Ocha' AS back, 'food' AS tags
UNION ALL
SELECT 'Delicious' AS front, 'Oishii' AS back, 'food' AS tags
UNION ALL
SELECT 'I don''t eat meat' AS front, 'Niku wa tabemasen' AS back, 'food, diet' AS tags
UNION ALL
SELECT 'The check, please' AS front, 'Okaikei o onegaishimasu' AS back, 'food' AS tags
UNION ALL
SELECT 'Toilet' AS front, 'Toire' AS back, 'place' AS tags
UNION ALL
SELECT 'Left' AS front, 'Hidari' AS back, 'directions' AS tags
UNION ALL
SELECT 'Right' AS front, 'Migi' AS back, 'directions' AS tags
UNION ALL
SELECT 'Straight' AS front, 'Massugu' AS back, 'directions' AS tags
UNION ALL
SELECT 'Today' AS front, 'Kyō' AS back, 'time' AS tags
UNION ALL
SELECT 'Tomorrow' AS front, 'Ashita' AS back, 'time' AS tags
UNION ALL
SELECT 'Yesterday' AS front, 'Kinō' AS back, 'time' AS tags
UNION ALL
SELECT 'What time?' AS front, 'Nan-ji desu ka?' AS back, 'time, question' AS tags
UNION ALL
SELECT 'My name is…' AS front, 'Watashi no namae wa … desu' AS back, 'introduction' AS tags
UNION ALL
SELECT 'Nice to meet you' AS front, 'Hajimemashite' AS back, 'introduction' AS tags
UNION ALL
SELECT 'Please treat me well' AS front, 'Yoroshiku onegaishimasu' AS back, 'introduction, polite' AS tags
UNION ALL
SELECT 'Goodbye' AS front, 'Sayōnara, or jaa ne with friends' AS back, 'greeting' AS tags
UNION ALL
SELECT 'See you' AS front, 'Mata ne' AS back, 'greeting' AS tags
UNION ALL
SELECT 'I don''t understand' AS front, 'Wakarimasen' AS back, 'conversation' AS tags
UNION ALL
SELECT 'Do you speak English?' AS front, 'Eigo o hanasemasu ka?' AS back, 'conversation' AS tags
UNION ALL
SELECT 'Help' AS front, 'Tasukete' AS back, 'emergency' AS tags
UNION ALL
SELECT 'Hospital' AS front, 'Byōin' AS back, 'place, health' AS tags
UNION ALL
SELECT 'Police' AS front, 'Keisatsu' AS back, 'emergency' AS tags
UNION ALL
SELECT 'Open' AS front, 'Eigyō-chū, or aite imasu' AS back, 'sign' AS tags
UNION ALL
SELECT 'Closed' AS front, 'Junbichū for preparing, or shimatte imasu' AS back, 'sign' AS tags
UNION ALL
SELECT 'One, two, three' AS front, 'Ichi, ni, san' AS back, 'number' AS tags
) AS card;
INSERT INTO decks (name, description, created_at) VALUES ('World Capitals', 'Country on the front, capital on the back. A few notes flag cities that surprise people.', date('now'));
INSERT INTO cards (deck_id, front, back, tags, card_type, choices, interval_days, ease_factor, repetition_count, next_review_date, last_reviewed_date)
SELECT deck.deck_id, card.front, card.back, card.tags, 0, '', 0, 2.5, 0, '', ''
FROM (SELECT last_insert_rowid() AS deck_id) AS deck
JOIN (
SELECT 'France' AS front, 'Paris' AS back, 'europe, capital' AS tags
UNION ALL
SELECT 'Germany' AS front, 'Berlin' AS back, 'europe, capital' AS tags
UNION ALL
SELECT 'Italy' AS front, 'Rome' AS back, 'europe, capital' AS tags
UNION ALL
SELECT 'Spain' AS front, 'Madrid' AS back, 'europe, capital' AS tags
UNION ALL
SELECT 'Portugal' AS front, 'Lisbon' AS back, 'europe, capital' AS tags
UNION ALL
SELECT 'Ireland' AS front, 'Dublin' AS back, 'europe, capital' AS tags
UNION ALL
SELECT 'United Kingdom' AS front, 'London' AS back, 'europe, capital' AS tags
UNION ALL
SELECT 'Norway' AS front, 'Oslo' AS back, 'europe, capital' AS tags
UNION ALL
SELECT 'Sweden' AS front, 'Stockholm' AS back, 'europe, capital' AS tags
UNION ALL
SELECT 'Finland' AS front, 'Helsinki' AS back, 'europe, capital' AS tags
UNION ALL
SELECT 'Poland' AS front, 'Warsaw' AS back, 'europe, capital' AS tags
UNION ALL
SELECT 'Ukraine' AS front, 'Kyiv' AS back, 'europe, capital' AS tags
UNION ALL
SELECT 'Greece' AS front, 'Athens' AS back, 'europe, capital' AS tags
UNION ALL
SELECT 'Switzerland' AS front, 'Bern' AS back, 'europe, capital' AS tags
UNION ALL
SELECT 'Iceland' AS front, 'Reykjavík' AS back, 'europe, capital' AS tags
UNION ALL
SELECT 'Turkey' AS front, 'Ankara. Istanbul is the largest city, not the capital.' AS back, 'asia, capital' AS tags
UNION ALL
SELECT 'Japan' AS front, 'Tokyo' AS back, 'asia, capital' AS tags
UNION ALL
SELECT 'China' AS front, 'Beijing' AS back, 'asia, capital' AS tags
UNION ALL
SELECT 'South Korea' AS front, 'Seoul' AS back, 'asia, capital' AS tags
UNION ALL
SELECT 'India' AS front, 'New Delhi' AS back, 'asia, capital' AS tags
UNION ALL
SELECT 'Thailand' AS front, 'Bangkok' AS back, 'asia, capital' AS tags
UNION ALL
SELECT 'Vietnam' AS front, 'Hanoi' AS back, 'asia, capital' AS tags
UNION ALL
SELECT 'Indonesia' AS front, 'Jakarta' AS back, 'asia, capital' AS tags
UNION ALL
SELECT 'Philippines' AS front, 'Manila' AS back, 'asia, capital' AS tags
UNION ALL
SELECT 'Australia' AS front, 'Canberra. Sydney and Melbourne are larger.' AS back, 'oceania, capital' AS tags
UNION ALL
SELECT 'New Zealand' AS front, 'Wellington' AS back, 'oceania, capital' AS tags
UNION ALL
SELECT 'Canada' AS front, 'Ottawa' AS back, 'americas, capital' AS tags
UNION ALL
SELECT 'Mexico' AS front, 'Mexico City' AS back, 'americas, capital' AS tags
UNION ALL
SELECT 'Brazil' AS front, 'Brasília' AS back, 'americas, capital' AS tags
UNION ALL
SELECT 'Argentina' AS front, 'Buenos Aires' AS back, 'americas, capital' AS tags
UNION ALL
SELECT 'Chile' AS front, 'Santiago' AS back, 'americas, capital' AS tags
UNION ALL
SELECT 'Peru' AS front, 'Lima' AS back, 'americas, capital' AS tags
UNION ALL
SELECT 'Colombia' AS front, 'Bogotá' AS back, 'americas, capital' AS tags
UNION ALL
SELECT 'Egypt' AS front, 'Cairo' AS back, 'africa, capital' AS tags
UNION ALL
SELECT 'Morocco' AS front, 'Rabat. Casablanca is larger.' AS back, 'africa, capital' AS tags
UNION ALL
SELECT 'Nigeria' AS front, 'Abuja. Lagos is the largest city.' AS back, 'africa, capital' AS tags
UNION ALL
SELECT 'Kenya' AS front, 'Nairobi' AS back, 'africa, capital' AS tags
UNION ALL
SELECT 'Ghana' AS front, 'Accra' AS back, 'africa, capital' AS tags
UNION ALL
SELECT 'South Africa' AS front, 'Pretoria is the executive capital. Cape Town and Bloemfontein share other roles.' AS back, 'africa, capital' AS tags
UNION ALL
SELECT 'Saudi Arabia' AS front, 'Riyadh' AS back, 'asia, capital' AS tags
) AS card;
INSERT INTO decks (name, description, created_at) VALUES ('Human Body', 'What each structure does, in one plain sentence. Front is the name, back is the job.', date('now'));
INSERT INTO cards (deck_id, front, back, tags, card_type, choices, interval_days, ease_factor, repetition_count, next_review_date, last_reviewed_date)
SELECT deck.deck_id, card.front, card.back, card.tags, 0, '', 0, 2.5, 0, '', ''
FROM (SELECT last_insert_rowid() AS deck_id) AS deck
JOIN (
SELECT 'Heart' AS front, 'Pumps blood through the lungs and the rest of the body.' AS back, 'organ, circulation' AS tags
UNION ALL
SELECT 'Lungs' AS front, 'Exchange oxygen and carbon dioxide with the blood.' AS back, 'organ, breathing' AS tags
UNION ALL
SELECT 'Trachea' AS front, 'The windpipe that carries air from the larynx to the bronchi.' AS back, 'breathing' AS tags
UNION ALL
SELECT 'Diaphragm' AS front, 'The muscle under the lungs that drives breathing.' AS back, 'breathing, muscle' AS tags
UNION ALL
SELECT 'Esophagus' AS front, 'The tube that carries swallowed food to the stomach.' AS back, 'digestion' AS tags
UNION ALL
SELECT 'Stomach' AS front, 'Churns food and begins protein digestion with acid and enzymes.' AS back, 'digestion, organ' AS tags
UNION ALL
SELECT 'Small intestine' AS front, 'Where most digestion finishes and nutrients are absorbed.' AS back, 'digestion' AS tags
UNION ALL
SELECT 'Large intestine' AS front, 'Absorbs water and forms the remaining waste into stool.' AS back, 'digestion' AS tags
UNION ALL
SELECT 'Liver' AS front, 'Makes bile, stores glycogen, and processes many substances from the blood.' AS back, 'organ, digestion' AS tags
UNION ALL
SELECT 'Gallbladder' AS front, 'Stores and releases bile made by the liver.' AS back, 'digestion' AS tags
UNION ALL
SELECT 'Pancreas' AS front, 'Releases digestive enzymes and the hormones insulin and glucagon.' AS back, 'organ, digestion' AS tags
UNION ALL
SELECT 'Kidneys' AS front, 'Filter blood, balance water and salts, and make urine.' AS back, 'organ, excretion' AS tags
UNION ALL
SELECT 'Spleen' AS front, 'Filters blood and helps the immune system recycle old red cells.' AS back, 'organ, immune' AS tags
UNION ALL
SELECT 'Brain' AS front, 'Receives senses, plans movement, and holds memory and thought.' AS back, 'organ, nervous' AS tags
UNION ALL
SELECT 'Neuron' AS front, 'A nerve cell that sends electrical and chemical signals.' AS back, 'nervous' AS tags
UNION ALL
SELECT 'Spinal cord' AS front, 'Carries signals between the brain and the body and runs some reflexes.' AS back, 'nervous' AS tags
UNION ALL
SELECT 'Aorta' AS front, 'The largest artery, carrying oxygen-rich blood out of the left ventricle.' AS back, 'circulation' AS tags
UNION ALL
SELECT 'Vena cava' AS front, 'The large veins that return blood to the right atrium.' AS back, 'circulation' AS tags
UNION ALL
SELECT 'Artery' AS front, 'A vessel that carries blood away from the heart.' AS back, 'circulation' AS tags
UNION ALL
SELECT 'Vein' AS front, 'A vessel that carries blood back toward the heart.' AS back, 'circulation' AS tags
UNION ALL
SELECT 'Capillary' AS front, 'A tiny vessel where gases and nutrients pass into tissues.' AS back, 'circulation' AS tags
UNION ALL
SELECT 'Red blood cell' AS front, 'Carries oxygen using hemoglobin.' AS back, 'blood' AS tags
UNION ALL
SELECT 'White blood cell' AS front, 'A cell of the immune system that fights infection.' AS back, 'blood, immune' AS tags
UNION ALL
SELECT 'Platelet' AS front, 'A cell fragment that helps blood clot.' AS back, 'blood' AS tags
UNION ALL
SELECT 'Skin' AS front, 'The body''s outer barrier, also used for touch and temperature control.' AS back, 'organ' AS tags
UNION ALL
SELECT 'Femur' AS front, 'The thigh bone, the longest bone in the body.' AS back, 'bone' AS tags
UNION ALL
SELECT 'Humerus' AS front, 'The bone of the upper arm.' AS back, 'bone' AS tags
UNION ALL
SELECT 'Tibia' AS front, 'The larger shin bone.' AS back, 'bone' AS tags
UNION ALL
SELECT 'Radius' AS front, 'The forearm bone on the thumb side.' AS back, 'bone' AS tags
UNION ALL
SELECT 'Ulna' AS front, 'The forearm bone on the little-finger side.' AS back, 'bone' AS tags
UNION ALL
SELECT 'Skull' AS front, 'The bones that protect the brain and form the face.' AS back, 'bone' AS tags
UNION ALL
SELECT 'Ribs' AS front, 'Curved bones that protect the heart and lungs.' AS back, 'bone' AS tags
UNION ALL
SELECT 'Pelvis' AS front, 'The hip bones that support the trunk and cradle organs.' AS back, 'bone' AS tags
UNION ALL
SELECT 'Tendon' AS front, 'Connects muscle to bone.' AS back, 'connective' AS tags
UNION ALL
SELECT 'Ligament' AS front, 'Connects bone to bone at a joint.' AS back, 'connective' AS tags
UNION ALL
SELECT 'Cartilage' AS front, 'Smooth, flexible tissue that cushions many joints.' AS back, 'connective' AS tags
UNION ALL
SELECT 'Cornea' AS front, 'The clear front window of the eye that bends incoming light.' AS back, 'eye' AS tags
UNION ALL
SELECT 'Retina' AS front, 'The light-sensitive layer at the back of the eye.' AS back, 'eye' AS tags
UNION ALL
SELECT 'Cochlea' AS front, 'The spiral inner-ear organ that turns sound into nerve signals.' AS back, 'ear' AS tags
UNION ALL
SELECT 'Skin''s epidermis' AS front, 'The outer layer of skin, constantly replacing itself.' AS back, 'skin' AS tags
) AS card;
INSERT INTO decks (name, description, created_at) VALUES ('Elements You Meet', 'Symbol, name, and one reason the element shows up in ordinary life.', date('now'));
INSERT INTO cards (deck_id, front, back, tags, card_type, choices, interval_days, ease_factor, repetition_count, next_review_date, last_reviewed_date)
SELECT deck.deck_id, card.front, card.back, card.tags, 0, '', 0, 2.5, 0, '', ''
FROM (SELECT last_insert_rowid() AS deck_id) AS deck
JOIN (
SELECT 'H' AS front, 'Hydrogen. The lightest element, and the fuel of stars.' AS back, 'symbol, gas' AS tags
UNION ALL
SELECT 'He' AS front, 'Helium. An unreactive gas used in balloons and in cooling magnets.' AS back, 'symbol, gas' AS tags
UNION ALL
SELECT 'Li' AS front, 'Lithium. A soft metal used in rechargeable batteries.' AS back, 'symbol, metal' AS tags
UNION ALL
SELECT 'Be' AS front, 'Beryllium. A light, stiff metal used in aerospace parts.' AS back, 'symbol, metal' AS tags
UNION ALL
SELECT 'B' AS front, 'Boron. Used in borosilicate glass and in some detergents.' AS back, 'symbol' AS tags
UNION ALL
SELECT 'C' AS front, 'Carbon. The backbone of living molecules, and of coal, graphite, and diamond.' AS back, 'symbol, life' AS tags
UNION ALL
SELECT 'N' AS front, 'Nitrogen. Most of the air, and a key ingredient of proteins.' AS back, 'symbol, gas' AS tags
UNION ALL
SELECT 'O' AS front, 'Oxygen. The gas cells use to release energy from food.' AS back, 'symbol, gas' AS tags
UNION ALL
SELECT 'F' AS front, 'Fluorine. The most reactive halogen, added to some toothpastes as fluoride.' AS back, 'symbol' AS tags
UNION ALL
SELECT 'Ne' AS front, 'Neon. An inert gas that glows red-orange in signs.' AS back, 'symbol, gas' AS tags
UNION ALL
SELECT 'Na' AS front, 'Sodium. A metal that, as an ion, is essential in nerves and in table salt.' AS back, 'symbol, metal' AS tags
UNION ALL
SELECT 'Mg' AS front, 'Magnesium. A light metal in alloys, and a mineral in chlorophyll.' AS back, 'symbol, metal' AS tags
UNION ALL
SELECT 'Al' AS front, 'Aluminum. A light metal used for cans, foil, and aircraft.' AS back, 'symbol, metal' AS tags
UNION ALL
SELECT 'Si' AS front, 'Silicon. The semiconductor at the heart of computer chips, and a major part of sand.' AS back, 'symbol' AS tags
UNION ALL
SELECT 'P' AS front, 'Phosphorus. Needed in DNA, ATP, and bones.' AS back, 'symbol, life' AS tags
UNION ALL
SELECT 'S' AS front, 'Sulfur. A yellow solid found in proteins and in some minerals.' AS back, 'symbol' AS tags
UNION ALL
SELECT 'Cl' AS front, 'Chlorine. A reactive gas used, carefully, to disinfect water.' AS back, 'symbol, gas' AS tags
UNION ALL
SELECT 'Ar' AS front, 'Argon. An inert gas used as a protective atmosphere in welding.' AS back, 'symbol, gas' AS tags
UNION ALL
SELECT 'K' AS front, 'Potassium. An ion that nerves and muscles cannot do without.' AS back, 'symbol, metal' AS tags
UNION ALL
SELECT 'Ca' AS front, 'Calcium. The metal that makes bones and teeth hard.' AS back, 'symbol, metal' AS tags
UNION ALL
SELECT 'Ti' AS front, 'Titanium. A strong, light, corrosion-resistant metal.' AS back, 'symbol, metal' AS tags
UNION ALL
SELECT 'Cr' AS front, 'Chromium. Used to plate metal and in stainless steel.' AS back, 'symbol, metal' AS tags
UNION ALL
SELECT 'Mn' AS front, 'Manganese. Added to steel to make it tougher.' AS back, 'symbol, metal' AS tags
UNION ALL
SELECT 'Fe' AS front, 'Iron. The metal of steel, and the center of hemoglobin.' AS back, 'symbol, metal' AS tags
UNION ALL
SELECT 'Co' AS front, 'Cobalt. Used in magnets and in some blue pigments.' AS back, 'symbol, metal' AS tags
UNION ALL
SELECT 'Ni' AS front, 'Nickel. A tough metal in coins and stainless steel.' AS back, 'symbol, metal' AS tags
UNION ALL
SELECT 'Cu' AS front, 'Copper. An excellent conductor used in electrical wire.' AS back, 'symbol, metal' AS tags
UNION ALL
SELECT 'Zn' AS front, 'Zinc. Coats steel to stop rust, and is a needed trace mineral.' AS back, 'symbol, metal' AS tags
UNION ALL
SELECT 'Br' AS front, 'Bromine. A reddish liquid halogen at room temperature.' AS back, 'symbol' AS tags
UNION ALL
SELECT 'Kr' AS front, 'Krypton. An inert gas used in some high-performance lamps.' AS back, 'symbol, gas' AS tags
UNION ALL
SELECT 'Ag' AS front, 'Silver. The best electrical conductor among the metals.' AS back, 'symbol, metal' AS tags
UNION ALL
SELECT 'Sn' AS front, 'Tin. A soft metal used to coat steel cans.' AS back, 'symbol, metal' AS tags
UNION ALL
SELECT 'I' AS front, 'Iodine. Needed by the thyroid, and used as an antiseptic.' AS back, 'symbol' AS tags
UNION ALL
SELECT 'Xe' AS front, 'Xenon. A heavy inert gas used in some headlights and anesthesia.' AS back, 'symbol, gas' AS tags
UNION ALL
SELECT 'Pt' AS front, 'Platinum. A rare metal used in catalytic converters and jewelry.' AS back, 'symbol, metal' AS tags
UNION ALL
SELECT 'Au' AS front, 'Gold. A dense, unreactive metal prized for electronics and jewelry.' AS back, 'symbol, metal' AS tags
UNION ALL
SELECT 'Hg' AS front, 'Mercury. The only metal that is liquid at room temperature.' AS back, 'symbol, metal' AS tags
UNION ALL
SELECT 'Pb' AS front, 'Lead. A dense, soft metal. Exposure is toxic, especially to children.' AS back, 'symbol, metal' AS tags
UNION ALL
SELECT 'Rn' AS front, 'Radon. A radioactive noble gas that can collect in basements.' AS back, 'symbol, gas' AS tags
UNION ALL
SELECT 'U' AS front, 'Uranium. A heavy radioactive metal used as nuclear fuel.' AS back, 'symbol, metal' AS tags
) AS card;
INSERT INTO decks (name, description, created_at) VALUES ('Reading Music', 'The marks on a page of music, and what they tell a player to do.', date('now'));
INSERT INTO cards (deck_id, front, back, tags, card_type, choices, interval_days, ease_factor, repetition_count, next_review_date, last_reviewed_date)
SELECT deck.deck_id, card.front, card.back, card.tags, 0, '', 0, 2.5, 0, '', ''
FROM (SELECT last_insert_rowid() AS deck_id) AS deck
JOIN (
SELECT 'Staff' AS front, 'The five lines and four spaces where notes sit.' AS back, 'notation' AS tags
UNION ALL
SELECT 'Treble clef' AS front, 'The G clef. It puts G above middle C on the second line.' AS back, 'clef' AS tags
UNION ALL
SELECT 'Bass clef' AS front, 'The F clef. It puts F below middle C on the fourth line.' AS back, 'clef' AS tags
UNION ALL
SELECT 'Alto clef' AS front, 'A C clef that puts middle C on the middle line. Viola music uses it.' AS back, 'clef' AS tags
UNION ALL
SELECT 'Whole note' AS front, 'An open notehead worth four beats in 4/4.' AS back, 'rhythm' AS tags
UNION ALL
SELECT 'Half note' AS front, 'An open notehead with a stem, worth two beats in 4/4.' AS back, 'rhythm' AS tags
UNION ALL
SELECT 'Quarter note' AS front, 'A filled notehead with a stem, worth one beat in 4/4.' AS back, 'rhythm' AS tags
UNION ALL
SELECT 'Eighth note' AS front, 'A quarter note with one flag, worth half a beat in 4/4.' AS back, 'rhythm' AS tags
UNION ALL
SELECT 'Sixteenth note' AS front, 'A note with two flags, worth a quarter of a beat in 4/4.' AS back, 'rhythm' AS tags
UNION ALL
SELECT 'Whole rest' AS front, 'A rest hanging from the fourth line, worth four beats in 4/4.' AS back, 'rhythm' AS tags
UNION ALL
SELECT 'Half rest' AS front, 'A rest sitting on the third line, worth two beats in 4/4.' AS back, 'rhythm' AS tags
UNION ALL
SELECT 'Quarter rest' AS front, 'A squiggle rest worth one beat.' AS back, 'rhythm' AS tags
UNION ALL
SELECT 'Dot after a note' AS front, 'Adds half the note''s value again. A dotted half note is three beats.' AS back, 'rhythm' AS tags
UNION ALL
SELECT 'Tie' AS front, 'A curved line joining two notes of the same pitch into one longer sound.' AS back, 'rhythm' AS tags
UNION ALL
SELECT 'Time signature 4/4' AS front, 'Four quarter-note beats in each measure.' AS back, 'meter' AS tags
UNION ALL
SELECT 'Time signature 3/4' AS front, 'Three quarter-note beats in each measure. A waltz meter.' AS back, 'meter' AS tags
UNION ALL
SELECT 'Time signature 6/8' AS front, 'Six eighth-note beats, usually felt as two groups of three.' AS back, 'meter' AS tags
UNION ALL
SELECT 'Bar line' AS front, 'A vertical line that marks the end of a measure.' AS back, 'notation' AS tags
UNION ALL
SELECT 'Double bar' AS front, 'Two bar lines that mark the end of a section or piece.' AS back, 'notation' AS tags
UNION ALL
SELECT 'Sharp' AS front, 'Raises a note by one half step.' AS back, 'pitch' AS tags
UNION ALL
SELECT 'Flat' AS front, 'Lowers a note by one half step.' AS back, 'pitch' AS tags
UNION ALL
SELECT 'Natural' AS front, 'Cancels a sharp or flat.' AS back, 'pitch' AS tags
UNION ALL
SELECT 'Key signature' AS front, 'The sharps or flats printed at the start, applied to every matching note.' AS back, 'pitch' AS tags
UNION ALL
SELECT 'Scale' AS front, 'A set of pitches in order, such as the major or natural minor scale.' AS back, 'pitch' AS tags
UNION ALL
SELECT 'Interval' AS front, 'The distance between two pitches, named by how many letter names it spans.' AS back, 'pitch' AS tags
UNION ALL
SELECT 'Octave' AS front, 'The interval of eight letter names. The notes sound like the same pitch class.' AS back, 'pitch' AS tags
UNION ALL
SELECT 'Chord' AS front, 'Three or more notes sounding together.' AS back, 'harmony' AS tags
UNION ALL
SELECT 'Triad' AS front, 'A three-note chord built from two stacked thirds.' AS back, 'harmony' AS tags
UNION ALL
SELECT 'Major triad' AS front, 'A chord with a major third on the bottom and a minor third on top.' AS back, 'harmony' AS tags
UNION ALL
SELECT 'Minor triad' AS front, 'A chord with a minor third on the bottom and a major third on top.' AS back, 'harmony' AS tags
UNION ALL
SELECT 'Forte' AS front, 'Play loud. Abbreviated f.' AS back, 'dynamics' AS tags
UNION ALL
SELECT 'Piano, the dynamic' AS front, 'Play soft. Abbreviated p.' AS back, 'dynamics' AS tags
UNION ALL
SELECT 'Crescendo' AS front, 'Gradually get louder.' AS back, 'dynamics' AS tags
UNION ALL
SELECT 'Decrescendo' AS front, 'Gradually get softer.' AS back, 'dynamics' AS tags
UNION ALL
SELECT 'Tempo' AS front, 'The speed of the beat.' AS back, 'tempo' AS tags
UNION ALL
SELECT 'Allegro' AS front, 'A fast, lively tempo.' AS back, 'tempo' AS tags
UNION ALL
SELECT 'Andante' AS front, 'A walking tempo, moderately slow.' AS back, 'tempo' AS tags
UNION ALL
SELECT 'Adagio' AS front, 'A slow tempo.' AS back, 'tempo' AS tags
UNION ALL
SELECT 'Fermata' AS front, 'Hold the note or rest longer than its written value.' AS back, 'expression' AS tags
UNION ALL
SELECT 'Staccato' AS front, 'A dot that means play the note short and detached.' AS back, 'expression' AS tags
) AS card;
INSERT INTO decks (name, description, created_at) VALUES ('Turning Points in U.S. History', 'An event on the front, the year and why it mattered on the back.', date('now'));
INSERT INTO cards (deck_id, front, back, tags, card_type, choices, interval_days, ease_factor, repetition_count, next_review_date, last_reviewed_date)
SELECT deck.deck_id, card.front, card.back, card.tags, 0, '', 0, 2.5, 0, '', ''
FROM (SELECT last_insert_rowid() AS deck_id) AS deck
JOIN (
SELECT 'Declaration of Independence' AS front, '1776. The thirteen colonies declared themselves independent of Britain.' AS back, 'founding' AS tags
UNION ALL
SELECT 'Constitution signed' AS front, '1787. Delegates in Philadelphia drafted the framework still used today.' AS back, 'founding' AS tags
UNION ALL
SELECT 'Bill of Rights ratified' AS front, '1791. The first ten amendments listed core individual liberties.' AS back, 'founding, rights' AS tags
UNION ALL
SELECT 'Louisiana Purchase' AS front, '1803. The United States bought a vast territory from France.' AS back, 'expansion' AS tags
UNION ALL
SELECT 'Lewis and Clark expedition' AS front, '1804–1806. They mapped a route from the Missouri River to the Pacific.' AS back, 'expansion' AS tags
UNION ALL
SELECT 'War of 1812 ends' AS front, '1815. The Treaty of Ghent had been signed in 1814; the war confirmed U.S. independence in practice.' AS back, 'war' AS tags
UNION ALL
SELECT 'Missouri Compromise' AS front, '1820. It tried to balance free and slave states as the country grew.' AS back, 'slavery' AS tags
UNION ALL
SELECT 'Indian Removal Act' AS front, '1830. It authorized forcing Native nations from the Southeast. The Trail of Tears followed.' AS back, 'expansion, injustice' AS tags
UNION ALL
SELECT 'Seneca Falls Convention' AS front, '1848. A meeting that launched an organized movement for women''s rights.' AS back, 'rights' AS tags
UNION ALL
SELECT 'Compromise of 1850' AS front, '1850. A package of laws, including a harsh Fugitive Slave Act, that delayed the crisis over slavery.' AS back, 'slavery' AS tags
UNION ALL
SELECT 'Dred Scott decision' AS front, '1857. The Supreme Court denied citizenship to Black people and struck at limits on slavery.' AS back, 'slavery, court' AS tags
UNION ALL
SELECT 'Civil War begins' AS front, '1861. Confederate forces fired on Fort Sumter.' AS back, 'war' AS tags
UNION ALL
SELECT 'Emancipation Proclamation' AS front, '1863. Lincoln declared enslaved people in rebelling states free.' AS back, 'slavery, war' AS tags
UNION ALL
SELECT 'Gettysburg Address' AS front, '1863. Lincoln framed the war as a test of whether a democracy could endure.' AS back, 'war' AS tags
UNION ALL
SELECT 'Civil War ends' AS front, '1865. Lee surrendered at Appomattox. Slavery was then abolished by the 13th Amendment.' AS back, 'war' AS tags
UNION ALL
SELECT '13th Amendment' AS front, '1865. It abolished slavery in the United States.' AS back, 'rights' AS tags
UNION ALL
SELECT '14th Amendment' AS front, '1868. It guaranteed citizenship and equal protection of the laws.' AS back, 'rights' AS tags
UNION ALL
SELECT '15th Amendment' AS front, '1870. It barred denying the vote on account of race.' AS back, 'rights' AS tags
UNION ALL
SELECT 'Transcontinental railroad completed' AS front, '1869. The rails met at Promontory Summit, Utah.' AS back, 'industry' AS tags
UNION ALL
SELECT 'Compromise of 1877' AS front, '1877. Federal troops left the South, and Reconstruction effectively ended.' AS back, 'reconstruction' AS tags
UNION ALL
SELECT 'Chinese Exclusion Act' AS front, '1882. The first major U.S. law to ban immigration by nationality.' AS back, 'immigration' AS tags
UNION ALL
SELECT 'Plessy v. Ferguson' AS front, '1896. The Court upheld racial segregation under ''separate but equal.''' AS back, 'court, rights' AS tags
UNION ALL
SELECT 'Spanish-American War' AS front, '1898. A short war after which the U.S. gained overseas territories.' AS back, 'war' AS tags
UNION ALL
SELECT 'Wright brothers'' first flight' AS front, '1903. The first powered, controlled airplane flight, at Kitty Hawk.' AS back, 'invention' AS tags
UNION ALL
SELECT '17th Amendment' AS front, '1913. It provided for direct election of senators.' AS back, 'reform' AS tags
UNION ALL
SELECT 'U.S. enters World War I' AS front, '1917. Congress declared war on Germany.' AS back, 'war' AS tags
UNION ALL
SELECT '19th Amendment' AS front, '1920. It prohibited denying the vote on account of sex.' AS back, 'rights' AS tags
UNION ALL
SELECT 'Stock market crash' AS front, '1929. The October crash helped trigger the Great Depression.' AS back, 'economy' AS tags
UNION ALL
SELECT 'Social Security Act' AS front, '1935. It created old-age insurance and other New Deal protections.' AS back, 'reform' AS tags
UNION ALL
SELECT 'Pearl Harbor attacked' AS front, '1941. The attack brought the United States into World War II.' AS back, 'war' AS tags
UNION ALL
SELECT 'D-Day' AS front, '1944. Allied forces landed in Normandy to open a western front.' AS back, 'war' AS tags
UNION ALL
SELECT 'Brown v. Board of Education' AS front, '1954. The Court ruled that segregated public schools are unconstitutional.' AS back, 'court, rights' AS tags
UNION ALL
SELECT 'Montgomery bus boycott' AS front, '1955–1956. A boycott after Rosa Parks''s arrest helped launch the mass civil rights movement.' AS back, 'rights' AS tags
UNION ALL
SELECT 'March on Washington' AS front, '1963. Hundreds of thousands gathered; King gave the ''I Have a Dream'' speech.' AS back, 'rights' AS tags
UNION ALL
SELECT 'Civil Rights Act' AS front, '1964. It outlawed discrimination in public accommodations and employment.' AS back, 'rights' AS tags
UNION ALL
SELECT 'Voting Rights Act' AS front, '1965. It banned practices used to keep Black citizens from voting.' AS back, 'rights' AS tags
UNION ALL
SELECT 'Moon landing' AS front, '1969. Apollo 11 put the first people on the Moon.' AS back, 'science' AS tags
UNION ALL
SELECT 'Watergate resignations' AS front, '1974. President Nixon resigned rather than face certain impeachment.' AS back, 'politics' AS tags
UNION ALL
SELECT 'Americans with Disabilities Act' AS front, '1990. It barred discrimination and required access in public life.' AS back, 'rights' AS tags
UNION ALL
SELECT 'September 11 attacks' AS front, '2001. Terrorist attacks killed nearly 3,000 people and reshaped U.S. policy.' AS back, 'war' AS tags
) AS card;
INSERT INTO decks (name, description, created_at) VALUES ('Python You Can Use', 'Small, true facts about the language. Front is a question or a snippet''s job, back is the answer.', date('now'));
INSERT INTO cards (deck_id, front, back, tags, card_type, choices, interval_days, ease_factor, repetition_count, next_review_date, last_reviewed_date)
SELECT deck.deck_id, card.front, card.back, card.tags, 0, '', 0, 2.5, 0, '', ''
FROM (SELECT last_insert_rowid() AS deck_id) AS deck
JOIN (
SELECT 'What does len("hi") return?' AS front, '2. len counts items in a sequence.' AS back, 'builtin' AS tags
UNION ALL
SELECT 'How do you start a comment?' AS front, 'With #. The rest of the line is ignored.' AS back, 'syntax' AS tags
UNION ALL
SELECT 'What is the type of 3.14?' AS front, 'float' AS back, 'type' AS tags
UNION ALL
SELECT 'What is the type of "3"?' AS front, 'str. Quotes make a string, even if the characters are digits.' AS back, 'type' AS tags
UNION ALL
SELECT 'How do you turn the string "12" into an int?' AS front, 'int("12")' AS back, 'conversion' AS tags
UNION ALL
SELECT 'What does 7 // 2 equal?' AS front, '3. // is floor division.' AS back, 'operator' AS tags
UNION ALL
SELECT 'What does 7 % 2 equal?' AS front, '1. % is the remainder.' AS back, 'operator' AS tags
UNION ALL
SELECT 'What does 2 ** 3 equal?' AS front, '8. ** is exponentiation.' AS back, 'operator' AS tags
UNION ALL
SELECT 'How do you write a list of three numbers?' AS front, '[1, 2, 3]' AS back, 'collection' AS tags
UNION ALL
SELECT 'How do you add an item to the end of a list?' AS front, 'items.append(value)' AS back, 'list' AS tags
UNION ALL
SELECT 'What does a list slice a[1:3] include?' AS front, 'Indexes 1 and 2. The stop index is excluded.' AS back, 'list' AS tags
UNION ALL
SELECT 'How do you write a dictionary of one pair?' AS front, '{"name": "Ada"}' AS back, 'collection' AS tags
UNION ALL
SELECT 'How do you get a dictionary value safely?' AS front, 'data.get("name"), which returns None if the key is missing.' AS back, 'dict' AS tags
UNION ALL
SELECT 'What does a tuple look like?' AS front, '(1, 2). Tuples cannot be changed after they are created.' AS back, 'collection' AS tags
UNION ALL
SELECT 'What does a set do with duplicates?' AS front, 'It keeps each value once. {1, 1, 2} is {1, 2}.' AS back, 'collection' AS tags
UNION ALL
SELECT 'How do you write an if / else?' AS front, 'if condition:\n    ...\nelse:\n    ...' AS back, 'control' AS tags
UNION ALL
SELECT 'What does range(3) produce?' AS front, '0, 1, 2. The stop value is not included.' AS back, 'loop' AS tags
UNION ALL
SELECT 'How do you loop over a list?' AS front, 'for item in items:' AS back, 'loop' AS tags
UNION ALL
SELECT 'What does break do?' AS front, 'It leaves the nearest loop immediately.' AS back, 'loop' AS tags
UNION ALL
SELECT 'What does continue do?' AS front, 'It skips the rest of this loop pass and starts the next one.' AS back, 'loop' AS tags
UNION ALL
SELECT 'How do you define a function?' AS front, 'def greet(name):' AS back, 'function' AS tags
UNION ALL
SELECT 'What does return do?' AS front, 'It sends a value back to the caller and ends the function.' AS back, 'function' AS tags
UNION ALL
SELECT 'What is None?' AS front, 'The value that means ''no value.'' Functions with no return give None.' AS back, 'type' AS tags
UNION ALL
SELECT 'How do you import the math module?' AS front, 'import math' AS back, 'module' AS tags
UNION ALL
SELECT 'How do you import one name?' AS front, 'from math import sqrt' AS back, 'module' AS tags
UNION ALL
SELECT 'What does f"{name}" do?' AS front, 'It builds a string and inserts the value of name.' AS back, 'string' AS tags
UNION ALL
SELECT 'How do you open a file for reading?' AS front, 'open(path) or, better, with open(path) as handle:' AS back, 'file' AS tags
UNION ALL
SELECT 'Why use with for a file?' AS front, 'It closes the file even if an error happens inside the block.' AS back, 'file' AS tags
UNION ALL
SELECT 'What does a try / except catch?' AS front, 'Exceptions raised in the try block. except names the kind you handle.' AS back, 'error' AS tags
UNION ALL
SELECT 'What is a list comprehension?' AS front, 'A compact way to build a list: [n * 2 for n in nums]' AS back, 'list' AS tags
UNION ALL
SELECT 'Are strings mutable?' AS front, 'No. Methods like replace return a new string.' AS back, 'string' AS tags
UNION ALL
SELECT 'Are lists mutable?' AS front, 'Yes. append and item assignment change the same list.' AS back, 'list' AS tags
UNION ALL
SELECT 'What does == test?' AS front, 'Whether two values are equal.' AS back, 'operator' AS tags
UNION ALL
SELECT 'What does is test?' AS front, 'Whether two names refer to the same object.' AS back, 'operator' AS tags
UNION ALL
SELECT 'What is a boolean?' AS front, 'True or False.' AS back, 'type' AS tags
UNION ALL
SELECT 'What does and require?' AS front, 'Both sides must be true.' AS back, 'logic' AS tags
UNION ALL
SELECT 'What does or require?' AS front, 'At least one side must be true.' AS back, 'logic' AS tags
UNION ALL
SELECT 'What does not do?' AS front, 'It flips True to False and False to True.' AS back, 'logic' AS tags
UNION ALL
SELECT 'How do you read input from the user?' AS front, 'input(prompt), which always returns a string.' AS back, 'builtin' AS tags
UNION ALL
SELECT 'What is PEP 8?' AS front, 'Python''s style guide for names, spacing, and layout.' AS back, 'style' AS tags
) AS card;
INSERT INTO decks (name, description, created_at) VALUES ('Seeing Art History', 'A movement or work on the front, and what to look for on the back.', date('now'));
INSERT INTO cards (deck_id, front, back, tags, card_type, choices, interval_days, ease_factor, repetition_count, next_review_date, last_reviewed_date)
SELECT deck.deck_id, card.front, card.back, card.tags, 0, '', 0, 2.5, 0, '', ''
FROM (SELECT last_insert_rowid() AS deck_id) AS deck
JOIN (
SELECT 'Renaissance' AS front, '1400s–1500s Europe. Artists revived classical ideas and studied anatomy, perspective, and natural light.' AS back, 'movement' AS tags
UNION ALL
SELECT 'Linear perspective' AS front, 'Parallel lines are drawn so they meet at a vanishing point, creating depth.' AS back, 'technique' AS tags
UNION ALL
SELECT 'Leonardo da Vinci' AS front, 'A Renaissance painter and investigator. The Mona Lisa and The Last Supper are his best-known paintings.' AS back, 'artist' AS tags
UNION ALL
SELECT 'Michelangelo''s David' AS front, 'A marble statue, finished in 1504, of the biblical hero before the fight.' AS back, 'work' AS tags
UNION ALL
SELECT 'Sistine Chapel ceiling' AS front, 'Michelangelo''s frescoes, painted 1508–1512, including the Creation of Adam.' AS back, 'work' AS tags
UNION ALL
SELECT 'Baroque' AS front, '1600s. Dramatic light, movement, and emotion. Caravaggio and Bernini are central.' AS back, 'movement' AS tags
UNION ALL
SELECT 'Caravaggio' AS front, 'A Baroque painter known for stark light against deep dark, called tenebrism.' AS back, 'artist' AS tags
UNION ALL
SELECT 'Rembrandt' AS front, 'A Dutch painter of portraits and biblical scenes, famous for warm light and honest faces.' AS back, 'artist' AS tags
UNION ALL
SELECT 'Vermeer' AS front, 'A Dutch painter of quiet interiors. Girl with a Pearl Earring is his most famous work.' AS back, 'artist' AS tags
UNION ALL
SELECT 'Rococo' AS front, 'Early 1700s. Light colors, curves, and playful scenes of leisure. Fragonard is a key name.' AS back, 'movement' AS tags
UNION ALL
SELECT 'Neoclassicism' AS front, 'Late 1700s. Calm, severe scenes drawn from Greece and Rome. Jacques-Louis David led it.' AS back, 'movement' AS tags
UNION ALL
SELECT 'Romanticism' AS front, 'Late 1700s–mid 1800s. Emotion, nature, and the sublime. Turner, Géricault, and Delacroix.' AS back, 'movement' AS tags
UNION ALL
SELECT 'J. M. W. Turner' AS front, 'A British painter of light, weather, and the sea, pushing toward abstraction.' AS back, 'artist' AS tags
UNION ALL
SELECT 'Realism' AS front, 'Mid 1800s. Ordinary workers and unidealized life. Courbet was a founder.' AS back, 'movement' AS tags
UNION ALL
SELECT 'Impressionism' AS front, '1870s France. Painters worked outdoors to catch changing light. Monet, Renoir, Degas.' AS back, 'movement' AS tags
UNION ALL
SELECT 'Claude Monet' AS front, 'An Impressionist who painted series of haystacks, the Rouen cathedral, and water lilies.' AS back, 'artist' AS tags
UNION ALL
SELECT 'Edgar Degas' AS front, 'An Impressionist known for dancers, racetracks, and unusual viewpoints.' AS back, 'artist' AS tags
UNION ALL
SELECT 'Post-Impressionism' AS front, 'Late 1800s. Artists kept bright color but built more personal structure. Van Gogh, Cézanne, Gauguin.' AS back, 'movement' AS tags
UNION ALL
SELECT 'Vincent van Gogh' AS front, 'A Post-Impressionist who used thick, swirling paint. The Starry Night dates to 1889.' AS back, 'artist' AS tags
UNION ALL
SELECT 'Paul Cézanne' AS front, 'He built landscapes from patches of color and strongly influenced Cubism.' AS back, 'artist' AS tags
UNION ALL
SELECT 'Art Nouveau' AS front, 'About 1890–1910. Organic lines in posters, architecture, and objects. Mucha and Gaudí.' AS back, 'movement' AS tags
UNION ALL
SELECT 'Fauvism' AS front, 'Early 1900s. Wild, non-natural color. Matisse was the leading figure.' AS back, 'movement' AS tags
UNION ALL
SELECT 'Cubism' AS front, 'From 1907. Objects broken into shifting planes. Picasso and Braque invented it.' AS back, 'movement' AS tags
UNION ALL
SELECT 'Pablo Picasso' AS front, 'A founder of Cubism who kept reinventing his style across a very long career.' AS back, 'artist' AS tags
UNION ALL
SELECT 'Les Demoiselles d''Avignon' AS front, 'Picasso, 1907. A painting that shattered single-point perspective.' AS back, 'work' AS tags
UNION ALL
SELECT 'Futurism' AS front, 'Early 1900s Italy. Art that celebrated speed, machines, and modern life.' AS back, 'movement' AS tags
UNION ALL
SELECT 'Dada' AS front, 'During World War I. Anti-art that mocked reason and taste. Duchamp''s readymades belong here.' AS back, 'movement' AS tags
UNION ALL
SELECT 'Fountain' AS front, 'Marcel Duchamp, 1917. A urinal signed and submitted as sculpture.' AS back, 'work' AS tags
UNION ALL
SELECT 'Surrealism' AS front, '1920s on. Dream logic and the unconscious. Dalí, Magritte, and Kahlo connect to it.' AS back, 'movement' AS tags
UNION ALL
SELECT 'Salvador Dalí' AS front, 'A Surrealist painter of melting clocks and precise, dreamlike scenes.' AS back, 'artist' AS tags
UNION ALL
SELECT 'Frida Kahlo' AS front, 'A Mexican painter of unsparing self-portraits tied to pain, identity, and politics.' AS back, 'artist' AS tags
UNION ALL
SELECT 'Abstract Expressionism' AS front, '1940s–1950s New York. Large gestures or fields of color. Pollock and Rothko.' AS back, 'movement' AS tags
UNION ALL
SELECT 'Jackson Pollock' AS front, 'He dripped and flung paint across canvases laid on the floor.' AS back, 'artist' AS tags
UNION ALL
SELECT 'Pop art' AS front, '1950s–1960s. Fine art made from ads, comics, and products. Warhol and Lichtenstein.' AS back, 'movement' AS tags
UNION ALL
SELECT 'Andy Warhol' AS front, 'A Pop artist who repeated soup cans, celebrities, and disasters like mass-produced images.' AS back, 'artist' AS tags
UNION ALL
SELECT 'Minimalism' AS front, '1960s. Simple geometric forms and industrial materials. Donald Judd is a key sculptor.' AS back, 'movement' AS tags
UNION ALL
SELECT 'Impression, Sunrise' AS front, 'Monet, 1872. The painting whose title a critic turned into the name Impressionism.' AS back, 'work' AS tags
UNION ALL
SELECT 'The Night Watch' AS front, 'Rembrandt, 1642. A huge civic-guard portrait full of movement and shadow.' AS back, 'work' AS tags
UNION ALL
SELECT 'Guernica' AS front, 'Picasso, 1937. A black, white, and gray mural protesting the bombing of a Basque town.' AS back, 'work' AS tags
UNION ALL
SELECT 'The Great Wave' AS front, 'Hokusai, about 1831. A Japanese woodblock print of a wave towering over boats, with Mount Fuji beyond.' AS back, 'work' AS tags
) AS card;
INSERT INTO decks (name, description, created_at) VALUES ('The Night Sky', 'What you are looking at, from the Moon out to the galaxies. One clear fact on each back.', date('now'));
INSERT INTO cards (deck_id, front, back, tags, card_type, choices, interval_days, ease_factor, repetition_count, next_review_date, last_reviewed_date)
SELECT deck.deck_id, card.front, card.back, card.tags, 0, '', 0, 2.5, 0, '', ''
FROM (SELECT last_insert_rowid() AS deck_id) AS deck
JOIN (
SELECT 'Light-year' AS front, 'The distance light travels in one year, about 9.5 trillion kilometers.' AS back, 'measure' AS tags
UNION ALL
SELECT 'Astronomical unit' AS front, 'The average Earth–Sun distance, about 150 million kilometers.' AS back, 'measure' AS tags
UNION ALL
SELECT 'Sun' AS front, 'The star at the center of our solar system. It is a yellow dwarf.' AS back, 'star' AS tags
UNION ALL
SELECT 'Moon' AS front, 'Earth''s natural satellite. The same side always faces Earth.' AS back, 'moon' AS tags
UNION ALL
SELECT 'Lunar phase' AS front, 'The shape of the sunlit part of the Moon we can see, from new to full and back.' AS back, 'moon' AS tags
UNION ALL
SELECT 'Solar eclipse' AS front, 'The Moon passes between Earth and the Sun and blocks the Sun''s light.' AS back, 'event' AS tags
UNION ALL
SELECT 'Lunar eclipse' AS front, 'Earth passes between the Sun and the Moon and Earth''s shadow falls on the Moon.' AS back, 'event' AS tags
UNION ALL
SELECT 'Mercury' AS front, 'The smallest planet and the closest to the Sun. A year there lasts 88 Earth days.' AS back, 'planet' AS tags
UNION ALL
SELECT 'Venus' AS front, 'The hottest planet, wrapped in a thick carbon-dioxide atmosphere.' AS back, 'planet' AS tags
UNION ALL
SELECT 'Earth' AS front, 'The only planet known to host life. It has liquid water and one large moon.' AS back, 'planet' AS tags
UNION ALL
SELECT 'Mars' AS front, 'A cold desert world with the largest volcano and canyon in the solar system.' AS back, 'planet' AS tags
UNION ALL
SELECT 'Jupiter' AS front, 'The largest planet, a gas giant with a lasting storm called the Great Red Spot.' AS back, 'planet' AS tags
UNION ALL
SELECT 'Saturn' AS front, 'A gas giant circled by bright rings of ice and rock.' AS back, 'planet' AS tags
UNION ALL
SELECT 'Uranus' AS front, 'An ice giant that rotates on its side.' AS back, 'planet' AS tags
UNION ALL
SELECT 'Neptune' AS front, 'The farthest known planet, an ice giant with fierce winds.' AS back, 'planet' AS tags
UNION ALL
SELECT 'Asteroid' AS front, 'A rocky body, mostly orbiting between Mars and Jupiter.' AS back, 'small body' AS tags
UNION ALL
SELECT 'Comet' AS front, 'An icy body that grows a tail of gas and dust when it nears the Sun.' AS back, 'small body' AS tags
UNION ALL
SELECT 'Meteor' AS front, 'The streak of light when a space rock burns in the atmosphere. The rock itself is a meteoroid.' AS back, 'small body' AS tags
UNION ALL
SELECT 'Meteorite' AS front, 'A space rock that survives the fall and reaches the ground.' AS back, 'small body' AS tags
UNION ALL
SELECT 'Star' AS front, 'A ball of gas so massive that nuclear fusion shines in its core.' AS back, 'star' AS tags
UNION ALL
SELECT 'Red giant' AS front, 'A star that has swollen and cooled after exhausting the hydrogen in its core.' AS back, 'star' AS tags
UNION ALL
SELECT 'White dwarf' AS front, 'The dense, cooling core left when a Sun-like star sheds its outer layers.' AS back, 'star' AS tags
UNION ALL
SELECT 'Supernova' AS front, 'The explosion of a massive star, briefly outshining its whole galaxy.' AS back, 'star' AS tags
UNION ALL
SELECT 'Neutron star' AS front, 'The city-sized, ultra-dense remnant of some supernovae.' AS back, 'star' AS tags
UNION ALL
SELECT 'Black hole' AS front, 'A region where gravity is so strong that nothing, not even light, can escape.' AS back, 'star' AS tags
UNION ALL
SELECT 'Galaxy' AS front, 'A huge system of stars, gas, dust, and dark matter, bound by gravity.' AS back, 'galaxy' AS tags
UNION ALL
SELECT 'Milky Way' AS front, 'The spiral galaxy that contains our solar system.' AS back, 'galaxy' AS tags
UNION ALL
SELECT 'Andromeda Galaxy' AS front, 'The nearest large galaxy to the Milky Way, about 2.5 million light-years away.' AS back, 'galaxy' AS tags
UNION ALL
SELECT 'Constellation' AS front, 'A named pattern of stars, and the region of sky around it.' AS back, 'sky' AS tags
UNION ALL
SELECT 'Orion' AS front, 'A bright winter constellation marked by three stars in a straight belt.' AS back, 'constellation' AS tags
UNION ALL
SELECT 'Polaris' AS front, 'The North Star. Earth''s north axis points near it, so it barely moves in the sky.' AS back, 'star' AS tags
UNION ALL
SELECT 'Big Dipper' AS front, 'A saucepan-shaped group of seven stars inside Ursa Major. It points toward Polaris.' AS back, 'constellation' AS tags
UNION ALL
SELECT 'Sirius' AS front, 'The brightest star in the night sky, in the constellation Canis Major.' AS back, 'star' AS tags
UNION ALL
SELECT 'Light pollution' AS front, 'Artificial light that washes out the night sky.' AS back, 'observing' AS tags
UNION ALL
SELECT 'Telescope' AS front, 'An instrument that gathers light so faint, distant objects can be seen.' AS back, 'observing' AS tags
UNION ALL
SELECT 'Reflecting telescope' AS front, 'A telescope that uses a mirror to collect light.' AS back, 'observing' AS tags
UNION ALL
SELECT 'Refracting telescope' AS front, 'A telescope that uses a lens to collect light.' AS back, 'observing' AS tags
UNION ALL
SELECT 'Exoplanet' AS front, 'A planet orbiting a star other than the Sun.' AS back, 'planet' AS tags
UNION ALL
SELECT 'Habitable zone' AS front, 'The range of distances from a star where liquid water could exist on a planet''s surface.' AS back, 'planet' AS tags
UNION ALL
SELECT 'International Space Station' AS front, 'A habitable laboratory in low Earth orbit, continuously crewed since 2000.' AS back, 'spaceflight' AS tags
) AS card;
INSERT INTO decks (name, description, created_at) VALUES ('Kitchen Fundamentals', 'Techniques, doneness, and a few rules that keep food both good and safe.', date('now'));
INSERT INTO cards (deck_id, front, back, tags, card_type, choices, interval_days, ease_factor, repetition_count, next_review_date, last_reviewed_date)
SELECT deck.deck_id, card.front, card.back, card.tags, 0, '', 0, 2.5, 0, '', ''
FROM (SELECT last_insert_rowid() AS deck_id) AS deck
JOIN (
SELECT 'Mise en place' AS front, 'Prepare and measure every ingredient before you start cooking.' AS back, 'habit' AS tags
UNION ALL
SELECT 'Sauté' AS front, 'Cook quickly in a little fat over fairly high heat, stirring often.' AS back, 'technique' AS tags
UNION ALL
SELECT 'Sweat' AS front, 'Cook vegetables gently in fat until soft, without browning them.' AS back, 'technique' AS tags
UNION ALL
SELECT 'Sear' AS front, 'Brown the surface of meat or vegetables over high heat.' AS back, 'technique' AS tags
UNION ALL
SELECT 'Braise' AS front, 'Brown food, then cook it slowly, partly covered, in a little liquid.' AS back, 'technique' AS tags
UNION ALL
SELECT 'Simmer' AS front, 'Cook in liquid just below a boil, with small lazy bubbles.' AS back, 'technique' AS tags
UNION ALL
SELECT 'Boil' AS front, 'Cook in liquid at a full rolling boil.' AS back, 'technique' AS tags
UNION ALL
SELECT 'Blanch' AS front, 'Boil briefly, then plunge into ice water to stop the cooking.' AS back, 'technique' AS tags
UNION ALL
SELECT 'Roast' AS front, 'Cook uncovered in the oven with dry heat, usually uncovered.' AS back, 'technique' AS tags
UNION ALL
SELECT 'Bake' AS front, 'Cook in the oven with dry heat. The word is used for breads, cakes, and casseroles.' AS back, 'technique' AS tags
UNION ALL
SELECT 'Broil' AS front, 'Cook with intense heat from above.' AS back, 'technique' AS tags
UNION ALL
SELECT 'Poach' AS front, 'Cook gently in liquid held below a simmer.' AS back, 'technique' AS tags
UNION ALL
SELECT 'Steam' AS front, 'Cook with the vapor from boiling water, not in the water itself.' AS back, 'technique' AS tags
UNION ALL
SELECT 'Deglaze' AS front, 'Add liquid to a hot pan to dissolve the browned bits into a sauce.' AS back, 'technique' AS tags
UNION ALL
SELECT 'Reduce' AS front, 'Boil a liquid so water evaporates and the flavor concentrates.' AS back, 'technique' AS tags
UNION ALL
SELECT 'Emulsify' AS front, 'Whisk two liquids that normally separate, such as oil and vinegar, into a stable mix.' AS back, 'technique' AS tags
UNION ALL
SELECT 'Dice' AS front, 'Cut into small, even cubes.' AS back, 'knife' AS tags
UNION ALL
SELECT 'Mince' AS front, 'Cut much finer than a dice.' AS back, 'knife' AS tags
UNION ALL
SELECT 'Julienne' AS front, 'Cut into thin matchsticks.' AS back, 'knife' AS tags
UNION ALL
SELECT 'Chiffonade' AS front, 'Stack leaves, roll them, and slice into thin ribbons.' AS back, 'knife' AS tags
UNION ALL
SELECT 'Al dente' AS front, 'Pasta cooked until tender but still firm to the bite.' AS back, 'doneness' AS tags
UNION ALL
SELECT 'Safe temperature for poultry' AS front, '165°F / 74°C in the thickest part.' AS back, 'safety' AS tags
UNION ALL
SELECT 'Danger zone' AS front, 'Between 40°F and 140°F (4°C and 60°C), where bacteria grow quickly.' AS back, 'safety' AS tags
UNION ALL
SELECT 'Refrigerate leftovers within' AS front, 'Two hours, or one hour if the room is above 90°F / 32°C.' AS back, 'safety' AS tags
UNION ALL
SELECT 'Salt early for beans and meat' AS front, 'Salt draws out moisture at first, then helps food stay juicy and taste seasoned through.' AS back, 'seasoning' AS tags
UNION ALL
SELECT 'Bloom spices' AS front, 'Warm ground spices briefly in fat so their flavor opens up.' AS back, 'seasoning' AS tags
UNION ALL
SELECT 'Taste as you go' AS front, 'Season in layers and taste before serving. You cannot fix what you never checked.' AS back, 'habit' AS tags
UNION ALL
SELECT 'Rest a roast' AS front, 'Let it sit after cooking so the juices settle back into the meat.' AS back, 'doneness' AS tags
UNION ALL
SELECT 'Carryover cooking' AS front, 'Food keeps cooking for a few minutes after it leaves the heat.' AS back, 'doneness' AS tags
UNION ALL
SELECT 'Soft peak' AS front, 'Whipped cream or egg white that flops over when you lift the whisk.' AS back, 'baking' AS tags
UNION ALL
SELECT 'Stiff peak' AS front, 'A whip that stands up straight when you lift the whisk.' AS back, 'baking' AS tags
UNION ALL
SELECT 'Creaming' AS front, 'Beating butter and sugar until pale and fluffy, so a cake rises evenly.' AS back, 'baking' AS tags
UNION ALL
SELECT 'Fold' AS front, 'Combine a light mixture into a heavier one with a gentle lift, so you do not knock out air.' AS back, 'baking' AS tags
UNION ALL
SELECT 'Knead' AS front, 'Work dough until the gluten stretches and the dough becomes smooth and elastic.' AS back, 'baking' AS tags
UNION ALL
SELECT 'Proof' AS front, 'Let yeast dough rise until it grows in volume.' AS back, 'baking' AS tags
UNION ALL
SELECT 'Maillard reaction' AS front, 'The browning of proteins and sugars that creates roasted, savory flavor.' AS back, 'science' AS tags
UNION ALL
SELECT 'Acid balances fat' AS front, 'A squeeze of lemon or a splash of vinegar makes a rich dish taste brighter.' AS back, 'seasoning' AS tags
UNION ALL
SELECT 'Sharp knife is safer' AS front, 'A sharp blade bites instead of slipping, so you use less force.' AS back, 'knife, safety' AS tags
UNION ALL
SELECT 'Board for raw meat' AS front, 'Use a separate board, or wash it well, before cutting food you will eat raw.' AS back, 'safety' AS tags
UNION ALL
SELECT 'Salted water for pasta' AS front, 'It should taste pleasantly salty, like the sea, so the pasta is seasoned as it cooks.' AS back, 'habit' AS tags
) AS card;
COMMIT;
SELECT 'Seeded 12 decks, 480 cards.';
SELECT 'Database now has ' ||
	(SELECT COUNT(*) FROM decks) || ' decks and ' ||
	(SELECT COUNT(*) FROM cards) || ' cards.';
SQL
