# AccountingProject

Planning Document can be found [here](https://docs.google.com/document/d/1S41MVizXaoOmCtGrHVM9dGUagHNVVkNXtU67TZLCtyI/edit?usp=sharing)

First Working Demo:
Instructions on how to clone the repo, build the project, and create the working directory are included in the release v0.1.0 notes. This readme will be used to detail the format of the initial .ar file, along with notes about the release.

The .ar file begins by detailing the accounts that will be used over the financial year.
Each account will have 1 line in the following format: \<account name\>,\<the character d or c representing whether the account is a debit or credit account\>,\<the character a, l, s, r, or e representing asset, liability, stockholder's equity, revenue, or expense respectively\>,\<beginning balance of the account at year's start\>
When all accounts have been detailed, the next line will follow the format J,J,J,J to indicate to the program that the file is done listing accounts, and it is time to read the existing journal entries. 
Each journal entry also gets its own line in the following format: \<d or c\>,\<account name\>,\<amount\>,\<date in mm/dd/yyyy format\>,\<description of the transaction\>
That is all that is needed for the program to function. Note that there is no whitespace between any of the commas separating the values. In fact, the only whitespace should be new lines separating different accounts and journal entries, along with spaces in user-defined strings, such as account names or transaction descriptions.

This demo does not contain all the features planned for the project. This demo supports only viewing the ledger data of a specified account for the accounting year, adding new journal entries, and quickly viewing the total debits and credits at the time of request. It does not support creating new accounts during program execution, nor the usage or definition of aliases for existing accounts. It doesn't support contra-accounts of any kind (barring expenses), a dividends account, or creating financial statements. 
