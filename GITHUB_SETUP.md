# How to Push Your Project to GitHub

## Step 1: Create GitHub Repository (Web Interface)

1. **Go to GitHub**: https://github.com
2. **Sign in** to your account (or create one if needed)
3. **Click the "+" icon** in the top right corner
4. **Select "New repository"**
5. **Fill in the details**:
   - Repository name: `midicwkeyer` (or `rust-midicwkeyer`)
   - Description: `MIDI CW keyer with audio output - Rust implementation`
   - Visibility: Choose **Public** or **Private**
   - ⚠️ **DO NOT** initialize with README, .gitignore, or license
     (we already have these files locally)
6. **Click "Create repository"**

GitHub will show you a page with setup instructions - ignore those, follow the steps below instead.

## Step 2: Initialize Git Locally

Open a terminal and run:

```bash
cd /home/developer/rust/midicwkeyer

# Initialize git repository
git init

# Set your name and email (if not already configured)
git config user.name "Your Name"
git config user.email "your.email@example.com"

# Or set globally (for all projects)
git config --global user.name "Your Name"
git config --global user.email "your.email@example.com"
```

## Step 3: Add All Files

```bash
# Add all files (respects .gitignore)
git add .

# Check what will be committed
git status
```

You should see:
```
On branch main
Changes to be committed:
  (use "git rm --cached <file>..." to unstage)
        new file:   .cargo/config.toml
        new file:   .github/workflows/build.yml
        new file:   .gitignore
        new file:   Cargo.lock
        new file:   Cargo.toml
        new file:   README.md
        ... (and more files)
```

## Step 4: Create Initial Commit

```bash
git commit -m "Initial commit: MIDI CW keyer Rust implementation

- Converted from Python to Rust
- Cross-platform MIDI and audio support
- MSVC cross-compilation for Windows
- GitHub Actions for macOS builds
- Comprehensive documentation"
```

## Step 5: Add GitHub Remote

Replace `YOUR_USERNAME` with your actual GitHub username:

```bash
git remote add origin https://github.com/YOUR_USERNAME/midicwkeyer.git

# Example:
# git remote add origin https://github.com/df7cb/midicwkeyer.git
```

**Verify the remote:**
```bash
git remote -v
```

Should show:
```
origin  https://github.com/YOUR_USERNAME/midicwkeyer.git (fetch)
origin  https://github.com/YOUR_USERNAME/midicwkeyer.git (push)
```

## Step 6: Set Main Branch

```bash
# Rename master to main (modern convention)
git branch -M main
```

## Step 7: Push to GitHub

```bash
# Push to GitHub
git push -u origin main
```

You'll be prompted for your GitHub credentials:
- **Username**: Your GitHub username
- **Password**: Use a **Personal Access Token** (not your GitHub password!)

### Creating a Personal Access Token

If you don't have a token:
1. Go to: https://github.com/settings/tokens
2. Click "Generate new token" → "Generate new token (classic)"
3. Give it a name: "midicwkeyer development"
4. Select scopes:
   - ✅ `repo` (Full control of private repositories)
   - ✅ `workflow` (Update GitHub Action workflows)
5. Click "Generate token"
6. **Copy the token** (you won't see it again!)
7. Use this token as your password when pushing

## Step 8: Verify on GitHub

1. Go to: `https://github.com/YOUR_USERNAME/midicwkeyer`
2. You should see all your files
3. **Check GitHub Actions**:
   - Go to "Actions" tab
   - You should see a workflow running (building for all platforms!)
   - Wait 3-5 minutes for it to complete

## Step 9: Download macOS Binaries

Once GitHub Actions completes:

1. Go to: **Actions** tab
2. Click on the latest workflow run
3. Scroll down to **Artifacts** section
4. Download:
   - `midicwkeyer-linux-x86_64`
   - `midicwkeyer-macos-x86_64` (Intel Macs)
   - `midicwkeyer-macos-aarch64` (M1/M2/M3 Macs)
   - `midicwkeyer-windows-x86_64.exe`

## Common Issues and Solutions

### Issue 1: "Authentication failed"
**Solution**: Use Personal Access Token instead of password
```bash
# When prompted:
Username: your_github_username
Password: paste_your_personal_access_token_here
```

### Issue 2: "Repository not found"
**Solution**: Check the remote URL
```bash
git remote -v
# If wrong, update it:
git remote set-url origin https://github.com/CORRECT_USERNAME/midicwkeyer.git
```

### Issue 3: "Updates were rejected"
**Solution**: Pull first (if you initialized repo with README)
```bash
git pull origin main --allow-unrelated-histories
git push -u origin main
```

### Issue 4: SSH instead of HTTPS
If you prefer SSH:
```bash
# Generate SSH key (if you don't have one)
ssh-keygen -t ed25519 -C "your.email@example.com"

# Add to GitHub: Settings → SSH and GPG keys → New SSH key
# Paste contents of: ~/.ssh/id_ed25519.pub

# Use SSH remote
git remote set-url origin git@github.com:YOUR_USERNAME/midicwkeyer.git
```

## Quick Reference

```bash
# Initialize and push (complete workflow)
cd /home/developer/rust/midicwkeyer
git init
git add .
git commit -m "Initial commit"
git remote add origin https://github.com/YOUR_USERNAME/midicwkeyer.git
git branch -M main
git push -u origin main

# Future updates
git add .
git commit -m "Your commit message"
git push
```

## After Pushing

### Making Updates

```bash
# 1. Make your changes to the code
# 2. Check what changed
git status
git diff

# 3. Add changes
git add .

# 4. Commit with meaningful message
git commit -m "Add feature X" 

# 5. Push to GitHub
git push
```

### Checking Build Status

After each push, GitHub Actions will:
- Build for Linux
- Build for Windows
- Build for macOS (Intel + ARM)
- Create artifacts for download

Check progress: GitHub → Actions tab

## Repository Settings (Optional)

After pushing, you might want to:

1. **Add topics/tags**: Settings → Topics
   - Add: `rust`, `midi`, `cw`, `morse-code`, `ham-radio`, `audio`

2. **Update description**: Repository main page → About → ⚙️ icon
   - Description: "MIDI CW keyer with cross-platform audio output"
   - Website: (if you have one)
   - Add topics

3. **Enable Discussions**: Settings → Features → Discussions

4. **Create first Release**:
   ```bash
   git tag v0.1.0
   git push origin v0.1.0
   ```
   Then on GitHub: Releases → Draft a new release

## Next Steps

✅ Push code to GitHub (follow steps above)
✅ Wait for GitHub Actions to complete (~5 minutes)
✅ Download and test macOS binaries
✅ Create a release (v0.1.0) when ready
✅ Share with the ham radio community!

## Your Repository URL

After creating on GitHub, your project will be at:
```
https://github.com/YOUR_USERNAME/midicwkeyer
```

Replace `YOUR_USERNAME` with your actual GitHub username.

---

Need help? Check GitHub's documentation:
- https://docs.github.com/en/get-started/quickstart/create-a-repo
- https://docs.github.com/en/authentication/keeping-your-account-and-data-secure/creating-a-personal-access-token
