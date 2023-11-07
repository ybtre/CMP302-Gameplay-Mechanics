# CMP302-Gameplay-Mechanics

Reupload of my CMP302 project for University.

<br/>
![Header Image](demo.gif)

<br/>

# Report
## Purpose
More often than not if you play a game today there will be some form of resource management system and/or a cost associated with your actions. If you want to cast a spell, you will most likely need some form of Energy/Mana/Rage etc. If you want to construct a building, you will most likely need Wood, Stone, Gold or any other resource decided on by the developers. Even Cooldowns are a form of resource management, they help balance high impact vs low impact abilities, while at the same time encouraging use of different abilities. The aim of the project was to establish the basis for one such system which could later be extended and improved based on the needs of the developer.
The Project is a basic template of how to create one such system and a possible way of structuring it in code to allow it to be easily extended with features. The limiting factor being your imagination on how to use the given resources.

### Summary
The mechanics I have implemented is a player resource (Health/Mana) management and manipulation mechanic in similar fashion to MMORPG/ARPG/MOBA etc games. Diablo and Path of Exile served as inspiration. The mechanic is made up of several components:

- Health system, where:
    - Taking damage causes the player to lose HP
    - Picking up HP Potions recovers Player HP
    - Easy access to HP values
    
- Mana system, where:
    - Spells cost mana to cast
    - Spells cost mana to channel
    - Mana regenerates over time
    - Picking up Mana Potions recovers a flat Mana amount
    - Picking up Mana Regen Buffs increases mana recovery rate drastically for a limited period of time
    - Spells can only be cast if the Player has enough Mana
    
- Spell Cooldowns:
    - Single Cast Spells can only be cast while not on Cooldown
    - Channeling Spells do not have a Cooldown (although some games in the genre implement a Cooldown for some Channeling Spells and this can easily be added here as well)
    
- Health/Mana Orbs UI:
    - Used showcase current and max Player Health and Mana resources
    
- Multiple abilities/spells that can be Cast:
    - “Vortex” - a channeling ability which deals steady damage over time to enemies in it’s area of effect. Has an Initial Mana Cost associated with it on first use, later has a relatively small and fixed channeling cost/
    - “Rock Blast” - a single cast ability with high base damage, high mana cost and high cooldown.
    
- Pickups:
    - Health Potion which restores a flat amount of HP to the Player when picked up
    - Mana Potion which restores a flat amount of Mana to the Player when picked up
    - Mana Regen Buff which greatly increases Player Mana Regen for a short period
    - Explosive Bomb which damages and subtracts a flat amount of HP from the Player on contact

- Enemy Dummy
    - Has a detection radius for the player, where if the player is inside the enemy starts chasing the player
    - Has an attack radius, where if the player is inside the enemy starts attacking the player
	  - Attack has a separate hitbox
    - Health
    - Death conditions
    
### Spells
#### “Vortex”

Description:
A spell which is cast and channeled by the player. The Spell has an initial mana cost associated with it. When the player casts the spell the system first checks if the player has enough Mana available. If there is not enough Mana available, then the player can not cast the spell. If there is enough Mana, the initial cost of the spell is subtracted from the currently available Mana and is cast. If the player continues to hold down the button instead of releasing it, the spell starts channeling and numerous things happen. The spell starts draining mana at a set rate per second. What is typical for games with a Mana system is that while channeling a spell, the player Mana does not stop regenerating. The way the amount of Mana to be drained is calculated by subtracting the spell channeling cost from the base Mana regen value of the player. (If the passive Mana regen is 5 Mana Per Second, and the spell channeling cost is 10 Mana Per Second, then the actual Mana drain while channeling will be 5 - 10 = - 5 Mana Per Second. This the player will only lose 5 Mana Per Second, instead of 10 as the channeling cost). The reason it is done this way is because in games often both of those values change as the character gets stronger and/or progresses and improves his stats. In games such as Path of Exile there are numerous builds which rely on the player having enough passive Mana Regen to offset any cost of, sometimes numerous, channeling and/or single cast abilities and thus never actually losing Mana and being able to sustain your Mana resource.

While channeling the ability, it constantly deals damage over time to enemies in its area of effect. The Damage amount can of course be controlled by the developer and is usually a small amount which adds up the longer the ability is being channeled and is a reliable and consistent source of damage. Another thing that happens while the ability is being channeled is that the player movement speed is reduced. This is made in order to have an extra risk factor to using the ability which plays into the decision making the player goes through when using abilities. Is the cost of the ability worth it in this scenario and against these Enemies.

Requirements:
1. The player to have enough mana to cast the ability
2. The player to have enough mana to channel the ability
3. The player to be alive

#### “Rock Burst”

Description:
This is a single cast spell. It is also High Damage, High Cost and High Cooldown. Currently it is capable of killing the enemies in 1 to 2 Casts, in order to offset and balance that it has a high innate Mana Cost and a High Cooldown. Several things happen once the spell is activated. First things is to check if the player has enough Mana to cover the cost of the spell. Next up is to check if the ability is not on Cooldown. When those requirements have been met the spell is cast. When cast it deals high damage in an area around the player once. The spell also goes on cooldown and it is unavailable to cast again within that period. This is also another common spell archetype in games which is why it was chosen. 

Requirements:
1. The player to have enough mana to cast the spell
2. The spell to not be on cooldown
3. The player to be alive

### Pickups
#### Base Item Class
All pickups inherit from it, it provides:
- a collision box
- a mesh
- a particle component
- BeginPlay function with delegates for BeginOverlap and EndOverlap
- Tick function
- OnOverlapBegin Delegate function that handles spawning particles

#### Explosive “Pickup”
When the player interacts with the explosive, the player takes damage. Damage function is called OnOverlapBegin and is using a custom DecreaseHealth() function instead of using the provided apply/take damage functions by UGameplayStatistics. After the damage to the player has been dealt the object is destroyed. 

#### Health Potion Pickup
Functionally very similar to the Explosive, but instead of dealing damage in the OnOverlapBegin delegate, it gets the player’s current Health and adds the healing amount to it using the player SetCurrentHealth() function.

#### Mana Potion Pickup
The same as the Health Potion Pickup, but instead of getting the player’s current Health, it gets the player’s current Mana and adds the Mana potion amount to it using the player’s SetCurrentMana() function

#### Mana Regen Buff Pickup
This pickup changes the player’s Mana status enum state using the SetManaStatus() function in the OnOverlapBegin() delegate. That changes the state of the player’s mana regen from the normal passive amount to 5 times the passive amount for 2 seconds. When that period runs out, the player’s mana status is changed back to normal. All of that is controlled in the player class.

### Enemy

Description:
A basic enemy used to test spells dealing damage, as well as the player taking damage. Poor bear has sacrificed many lives for the greater cause of testing the developer’s mechanics.

Requirements:
1. The Enemy has an Aggro range, when the player enters it, the enemy starts chasing
the player until said player leaves the aggro range.
2. The Enemy has a Combat range, when the player enters it, the enemy attacks at the
player and attempts to hit him

### HUD

Description:
The Heads Up Display actively displays the player’s current/max Health/Mana. Both in numeric values and in Diablo-esque looking Orbs. The Ability toolbar showcases the keybindings for the available spells as well as the developer’s writing skills using a mouse. The Cooldown for the Rock Burst Ability is displayed via an AddOnScreenDebugMessage() because the process of laying out elements on the HUD blueprint does not like to cooperate. Some elements appear off-screen in the blueprint, yet in game they are where they are expected to be.

### Coding Standards

For the majority of the project the developer has stuck as close as possible to the Unreal Engine 4 coding standard. The code has been written with clarity and simplicity in mind first, allowing the systems to be as flexible and available as possible.

## Conclusion and Reflection
Overall I am happy with the project. It serves as a good template for future projects and systems similar to the ones implemented. The mechanics/systems were made with flexibility in code in mind. If I want to create a new spell I have most of the basic tools needed already set up. If I want to connect something to one of the resources I can easily add that as well. The issue though is scalability in a way. The thing I wanted to improve, but lacked the time, was make the systems more modular, even splitting the systems into components. Separate the different verifications in the Tick function into separate functions which could also be more reusable. There is some duplicate logic which could be taken out into functions as well. A shortcoming which I did not predict was just the huge size of the project and how importing assets into the project messes with the file organisation. I tried to organise it somewhat sensibly, but there is a lot more to wish on that front. Another thing I did not expect was just the HUD widgets. As I mentioned earlier in the report, it makes no sense, the text fields are shown out of the canvas in the HUD blueprint, but in game they are in place. Tweaking the positions for the text and HP/Mana orbs was just trial and error. Move them a bit to the right, compile, press Play and see if it’s where I want it.

As with any project a developer is never fully satisfied, I can always improve the code structure, I can always make the flow of the data easier to follow, track and… flow. I can always optimize just one more thing.
