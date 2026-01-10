#!/bin/bash
# GitHub Setup Helper Script
# This script helps you push your project to GitHub

set -e

echo "🚀 GitHub Setup Helper for MIDI CW Keyer"
echo "========================================"
echo

# Check if git is installed
if ! command -v git &> /dev/null; then
    echo "❌ Git is not installed. Please install git first:"
    echo "   sudo apt install git"
    exit 1
fi

echo "✅ Git is installed"
echo

# Check if already initialized
if [ -d .git ]; then
    echo "✅ Git repository already initialized"
else
    echo "📦 Initializing git repository..."
    git init
    echo "✅ Git repository initialized"
fi

echo

# Check git config
if ! git config user.name &> /dev/null; then
    echo "⚠️  Git user.name not configured"
    read -p "Enter your name: " username
    git config user.name "$username"
    echo "✅ Set git user.name to: $username"
fi

if ! git config user.email &> /dev/null; then
    echo "⚠️  Git user.email not configured"
    read -p "Enter your email: " useremail
    git config user.email "$useremail"
    echo "✅ Set git user.email to: $useremail"
fi

echo
echo "Current git configuration:"
echo "  Name:  $(git config user.name)"
echo "  Email: $(git config user.email)"
echo

# Check for remote
if git remote | grep -q origin; then
    echo "✅ Remote 'origin' already configured:"
    git remote -v
    echo
    read -p "Do you want to change it? (y/N): " change_remote
    if [[ $change_remote =~ ^[Yy]$ ]]; then
        read -p "Enter new GitHub repository URL: " repo_url
        git remote set-url origin "$repo_url"
        echo "✅ Remote updated"
    fi
else
    echo "⚠️  No remote configured yet"
    echo
    echo "First, create a repository on GitHub:"
    echo "  1. Go to: https://github.com/new"
    echo "  2. Repository name: midicwkeyer"
    echo "  3. Description: MIDI CW keyer with audio output - Rust implementation"
    echo "  4. Choose Public or Private"
    echo "  5. DO NOT initialize with README or .gitignore"
    echo "  6. Click 'Create repository'"
    echo
    read -p "Have you created the repository? (y/N): " created
    
    if [[ ! $created =~ ^[Yy]$ ]]; then
        echo "❌ Please create the repository first, then run this script again"
        exit 0
    fi
    
    echo
    read -p "Enter your GitHub username: " github_user
    read -p "Enter repository name (default: midicwkeyer): " repo_name
    repo_name=${repo_name:-midicwkeyer}
    
    repo_url="https://github.com/$github_user/$repo_name.git"
    echo
    echo "Adding remote: $repo_url"
    git remote add origin "$repo_url"
    echo "✅ Remote added"
fi

echo
echo "📝 Preparing to commit..."

# Check if there are changes to commit
if git diff --quiet && git diff --cached --quiet 2>/dev/null; then
    if git rev-parse HEAD >/dev/null 2>&1; then
        echo "✅ No changes to commit (already committed)"
    else
        echo "📦 Adding all files..."
        git add .
        echo "✅ Files staged"
    fi
else
    echo "📦 Adding all files..."
    git add .
    echo "✅ Files staged"
fi

echo
echo "Files to be committed:"
git status --short | head -20
if [ $(git status --short | wc -l) -gt 20 ]; then
    echo "... and more"
fi

echo
read -p "Proceed with commit? (y/N): " do_commit

if [[ ! $do_commit =~ ^[Yy]$ ]]; then
    echo "❌ Commit cancelled"
    exit 0
fi

# Check if we need to make initial commit
if ! git rev-parse HEAD >/dev/null 2>&1; then
    echo
    echo "📝 Creating initial commit..."
    git commit -m "Initial commit: MIDI CW keyer Rust implementation

- Converted from Python to Rust
- Cross-platform MIDI and audio support  
- MSVC cross-compilation for Windows
- GitHub Actions for macOS builds
- Comprehensive documentation"
    echo "✅ Initial commit created"
else
    echo
    read -p "Enter commit message: " commit_msg
    commit_msg=${commit_msg:-"Update project"}
    git commit -m "$commit_msg"
    echo "✅ Commit created"
fi

echo
echo "📤 Setting main branch..."
git branch -M main
echo "✅ On main branch"

echo
echo "🚀 Pushing to GitHub..."
echo
echo "⚠️  IMPORTANT: When prompted for password, use your Personal Access Token!"
echo "   Get one at: https://github.com/settings/tokens"
echo "   Required scopes: 'repo' and 'workflow'"
echo
read -p "Press Enter to continue..."

if git push -u origin main; then
    echo
    echo "🎉 SUCCESS! Your project is now on GitHub!"
    echo
    echo "Next steps:"
    echo "  1. Visit your repository: $(git remote get-url origin | sed 's/\.git$//')"
    echo "  2. Check Actions tab - builds are running for all platforms"
    echo "  3. Wait ~5 minutes for builds to complete"
    echo "  4. Download macOS binaries from Actions → Artifacts"
    echo
    echo "Repository URL: $(git remote get-url origin | sed 's/\.git$//')"
else
    echo
    echo "❌ Push failed"
    echo
    echo "Common solutions:"
    echo "  1. Make sure you're using a Personal Access Token (not password)"
    echo "  2. Check your token has 'repo' and 'workflow' scopes"
    echo "  3. Verify repository URL: $(git remote get-url origin)"
    echo
    echo "See GITHUB_SETUP.md for detailed troubleshooting"
fi
