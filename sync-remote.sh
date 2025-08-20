#!/bin/bash

# This script is used to push all the commits from the main gitlab repo to the github backup repo...

# Set strict mode
set -euo pipefail

# Define your remotes
BACKUP_REMOTE="ghub-back"

# Push all branches to backup
echo "Pushing branches to $BACKUP_REMOTE..."
git push "$BACKUP_REMOTE" --mirror

echo "Sync complete."