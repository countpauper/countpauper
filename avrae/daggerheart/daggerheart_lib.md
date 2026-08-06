# Daggerheart library 

## Introduction

The Daggerheart workshop uses this gvar to share daggerheart functions. They are shared between aliases to avoid code duplication. It could be used by other alias creatures, who want to implement daggerheart automation in Avrae. 
If the workshop is complete however there should be very little need for this. 

## Copyright 

Daggerheart is (C) Darlington press. This library and workshop implement only the functionaltiy described in the SRD under the DPCGL. The library and the alias code are is MIT licensed. 

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

## Contact 

e-mail: countpauper@gmail.com 

Discord (incl Avrae server): @Velglarn#4688

# Reference 

## Import 
`dict = sheet(var_name=None)` 

Import the daggerheart character sheet data and state. This structure is used in most other functions.
If no var_name is specified, the current character's name is used as a variable. T
The variable is read as a cvar. This assumes the current character is a Daggerheart character. Otherwise it will return None. 

## Export
`export(sheet, var_name=None, force=False)`

Store the daggerheart character sheet. This is used by aliases that modify the sheet or state, such as resting and leveling up.  
The default character name is used if none is specified. The sheet is saves as a character variable with that name. This assumed the current character is a Daggerheart character. 

## Roll duality dice

`duality(sheet, hope="1d12", fear="1d12", bonus="", advantages=0, disadvantages=0, experience=[])`

Roll the duality dice. 
* sheet: character sheet used to look up experience and permanent bonuses. This sheet is imported with `import(...)`
* hope: Replace the hope die string e.g. "1d20" for rolls where the die is replaced or "2d12kh1" for dice where multiple hope dice can be rolled and the highest picked. 
* fear: Replace the fear die string in the same way
* bonus: bonus string. eg "3" or "1d4"
* advantages: the number of advantage dice to roll 
* disadvantages: the number of disadvantage dice to roll 
* Experience: Names of experiences on the sheet to add the bonus of. Partial and case insensitive matching is used to match experience. 




